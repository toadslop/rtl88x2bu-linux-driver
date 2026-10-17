// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for chip_type → ODM IC lookup (W4-36). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_chip_odm_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	unsigned chip_type;
	unsigned expect;
};

u32 chip_type_to_odm_ic_type(u8 chip_type);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "chip_type", &tmp))
		return -1;
	v->chip_type = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = (unsigned)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_intf_chip_odm_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		u32 got = chip_type_to_odm_ic_type((u8)vectors[i].chip_type);

		if (got != (u32)vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: chip_type=%u got=%u expect=%u\n",
				vectors[i].name, vectors[i].chip_type, got,
				vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
