// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for chip version RF type → hal rf_type (W4-64). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_intf_chip_rftype_types.h"
#include "host_vector_json.h"

typedef uint8_t u8;

#define MAX_VECTORS 32
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	unsigned cv_rf_type;
	unsigned limit;
	int expect;
};

enum host_rf_type {
	HOST_RF_TYPE_MAX = 16,
};

enum host_rf_type chip_rftype_to_hal_rftype_oracle(u8 cv_rf_type, u8 limit);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "cv_rf_type", &tmp))
		return -1;
	v->cv_rf_type = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "limit", &tmp))
		return -1;
	v->limit = (unsigned)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_intf_chip_rftype_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		int got;

#ifdef RUST_HAL_INTF_CHIP_RFTYPE_ORACLE
		got = host_chip_rftype_to_hal_rftype((u8)vectors[i].cv_rf_type,
						     (u8)vectors[i].limit);
#else
		got = (int)(unsigned)chip_rftype_to_hal_rftype_oracle(
			(u8)vectors[i].cv_rf_type, (u8)vectors[i].limit);
#endif

		if (got != vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: cv_rf_type=%u limit=%u got=%d expect=%d\n",
				vectors[i].name, vectors[i].cv_rf_type,
				vectors[i].limit, got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
