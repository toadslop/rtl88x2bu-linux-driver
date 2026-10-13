// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for HAL_IsLegalChannel (W4-14). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64

struct host_hal_legal_channel_adapter {
	uint8_t wireless_mode;
};

typedef uint8_t BOOLEAN;

struct vector {
	char name[MAX_NAME];
	uint8_t wireless_mode;
	uint32_t channel;
	int expect;
};

BOOLEAN HAL_IsLegalChannel(void *adapter, uint32_t channel);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "wireless_mode", &tmp))
		return -1;
	v->wireless_mode = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "channel", &tmp))
		return -1;
	v->channel = (uint32_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect", &tmp))
		return -1;
	v->expect = tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_is_legal_channel_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct host_hal_legal_channel_adapter adapter = {
			.wireless_mode = vectors[i].wireless_mode,
		};
		BOOLEAN got = HAL_IsLegalChannel(&adapter, vectors[i].channel);

		if ((int)got != vectors[i].expect) {
			fprintf(stderr,
				"FAIL %s: mode=0x%02x ch=%u got=%d expect=%d\n",
				vectors[i].name, vectors[i].wireless_mode,
				vectors[i].channel, (int)got, vectors[i].expect);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
