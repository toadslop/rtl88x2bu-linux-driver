// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for drv→HALMAC mappers (W4-21). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct hw_port_vector {
	char name[MAX_NAME];
	unsigned hwport;
	unsigned expect;
};

struct network_vector {
	char name[MAX_NAME];
	unsigned type;
	unsigned expect;
};

unsigned hw_port_drv2halmac(unsigned hwport);
unsigned network_type_drv2halmac(uint8_t type);

static int parse_hw_port(const char *obj, size_t len, void *vec_void)
{
	struct hw_port_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "hwport", &tmp))
		return -1;
	v->hwport = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int parse_network(const char *obj, size_t len, void *vec_void)
{
	struct network_vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "type", &tmp))
		return -1;
	v->type = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

static int run_hw_port(const char *path)
{
	struct hw_port_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_hw_port, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = hw_port_drv2halmac(vectors[i].hwport);

		if (got != vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: hwport=%u got=%u expect=%u\n",
				vectors[i].name, vectors[i].hwport, got,
				vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu hw_port vectors from %s\n", count, path);
	return 0;
}

static int run_network(const char *path)
{
	struct network_vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_network, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got =
			network_type_drv2halmac((uint8_t)vectors[i].type);

		if (got != vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: type=%u got=%u expect=%u\n",
				vectors[i].name, vectors[i].type, got,
				vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu network vectors from %s\n", count, path);
	return 0;
}

int main(int argc, char **argv)
{
	const char *hw_path =
		argc > 1 ? argv[1] : "hal_halmac_hw_port_drv2halmac_vectors.json";
	const char *net_path =
		argc > 2 ? argv[2] : "hal_halmac_network_drv2halmac_vectors.json";

	if (run_hw_port(hw_path) || run_network(net_path))
		return 1;

	return 0;
}
