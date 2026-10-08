// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for hw_rate_to_m_rate (W4-01). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 128
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	unsigned hw_rate;
	unsigned expect;
};

uint8_t hw_rate_to_m_rate(uint8_t hw_rate);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "hw_rate", &tmp))
		return -1;
	v->hw_rate = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_hw_rate_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		uint8_t got = hw_rate_to_m_rate((uint8_t)vectors[i].hw_rate);

		if (got != (uint8_t)vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: hw_rate=0x%02x got=0x%02x expect=0x%02x\n",
				vectors[i].name, vectors[i].hw_rate, got,
				vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
