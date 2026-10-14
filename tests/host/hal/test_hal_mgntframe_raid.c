// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_get_mgntframe_raid (W4-16). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

typedef uint8_t u8;

#define MAX_VECTORS 16
#define MAX_NAME 64

struct host_hal_mgntframe_raid_adapter {
	uint8_t hw_type;
};

struct vector {
	char name[MAX_NAME];
	uint8_t hw_type;
	uint8_t network_type;
	int expect;
};

u8 rtw_get_mgntframe_raid(void *adapter, unsigned char network_type);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "hw_type", &tmp))
		return -1;
	v->hw_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "network_type", &tmp))
		return -1;
	v->network_type = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_mgntframe_raid_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_mgntframe_raid_adapter adapter = {
			.hw_type = vectors[i].hw_type,
		};
		u8 got = rtw_get_mgntframe_raid(&adapter, vectors[i].network_type);

		if ((int)got != vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: hw_type=%u net=0x%02x got=%u expect=%d\n",
				vectors[i].name, vectors[i].hw_type,
				vectors[i].network_type, got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
