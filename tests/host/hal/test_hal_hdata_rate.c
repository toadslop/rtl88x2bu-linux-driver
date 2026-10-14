// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for _HDATA_RATE / rtw_rust_hal_com_hdata_rate (W4-17). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define DESC_RATE_NUM 0x54
#define MAX_VECTORS 16
#define MAX_NAME 48
#define MAX_EXPECT 32

struct vector {
	char name[MAX_NAME];
	uint8_t hw_rate;
	char expect[MAX_EXPECT];
};

const char *rtw_rust_hal_com_hdata_rate(uint8_t hw_rate);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "hw_rate", &tmp))
		return -1;
	v->hw_rate = (uint8_t)tmp;
	if (host_json_parse_string_in(obj, len, "expect", v->expect, sizeof(v->expect)))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_hdata_rate_vectors.json";

	for (uint16_t r = 0; r <= DESC_RATE_NUM; r++) {
		const char *got = rtw_rust_hal_com_hdata_rate((uint8_t)r);

		if (!got || got[0] == '\0') {
			fprintf(stderr, "FAIL enumerate: rate=0x%02x empty\n", r);
			return 1;
		}
	}

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		const char *got = rtw_rust_hal_com_hdata_rate(vectors[i].hw_rate);

		if (!got || strcmp(got, vectors[i].expect) != 0) {
			fprintf(stderr,
				"FAIL %s: hw_rate=0x%02x got=%s expect=%s\n",
				vectors[i].name, vectors[i].hw_rate,
				got ? got : "(null)", vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu overflow vectors + full 0..DESC_RATE_NUM table from %s\n",
	       count, path);
	return 0;
}
