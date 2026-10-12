// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for reserved page count mappers (W4-11). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	unsigned a;
	unsigned b;
};

unsigned rsvd_page_num_drv2halmac(uint16_t num);
unsigned rsvd_page_num_halmac2drv(unsigned pg);

static int parse_drv(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "num", &tmp))
		return -1;
	v->a = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->b = (unsigned)tmp;
	return 0;
}

static int parse_halmac(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "pg", &tmp))
		return -1;
	v->a = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->b = (unsigned)tmp;
	return 0;
}

static int run_file(const char *path, int halmac2drv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      halmac2drv ? parse_halmac : parse_drv, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return -1;
	}

	for (size_t i = 0; i < count; i++) {
		unsigned got = halmac2drv ?
			rsvd_page_num_halmac2drv(vectors[i].a) :
			rsvd_page_num_drv2halmac((uint16_t)vectors[i].a);

		if (got != vectors[i].b) {
			fprintf(stderr, "FAIL %s %s: got=%u expect=%u\n",
				halmac2drv ? "halmac2drv" : "drv2halmac",
				vectors[i].name, got, vectors[i].b);
			return -1;
		}
	}

	printf("PASS %s: %zu vectors from %s\n",
	       halmac2drv ? "halmac2drv" : "drv2halmac", count, path);
	return 0;
}

static int run_round_trip(void)
{
	static const uint16_t in[] = { 0, 8, 16, 24, 32, 64, 128, 256, 300 };
	static const uint16_t back[] = { 8, 8, 16, 24, 32, 64, 128, 256, 256 };

	for (size_t i = 0; i < sizeof(in) / sizeof(in[0]); i++) {
		uint16_t got = (uint16_t)rsvd_page_num_halmac2drv(
			rsvd_page_num_drv2halmac(in[i]));

		if (got != back[i]) {
			fprintf(stderr,
				"FAIL round-trip in=%u back=%u expect=%u\n",
				in[i], got, back[i]);
			return -1;
		}
	}

	printf("PASS round-trip: %zu cases\n", sizeof(in) / sizeof(in[0]));
	return 0;
}

int main(int argc, char **argv)
{
	const char *drv = argc > 1 ? argv[1] :
				 "hal_halmac_rsvd_drv2halmac_vectors.json";
	const char *halmac = argc > 2 ? argv[2] :
					  "hal_halmac_rsvd_halmac2drv_vectors.json";

	if (run_file(drv, 0) || run_file(halmac, 1) || run_round_trip())
		return 1;
	return 0;
}
