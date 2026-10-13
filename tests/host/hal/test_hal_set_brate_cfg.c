// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for HalSetBrateCfg (W4-13). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 64
#define RATES_LEN 16

struct vector {
	char name[MAX_NAME];
	uint8_t rates[RATES_LEN];
	uint16_t init_mask;
	uint16_t expect_mask;
};

void HalSetBrateCfg(void *adapter, uint8_t *mBratesOS, uint16_t *pBrateCfg);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	size_t decoded = 0;
	int tmp;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "rates_hex", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->rates, sizeof(v->rates), &decoded))
		return -1;
	if (decoded != RATES_LEN) {
		fprintf(stderr, "rates_hex must decode to %d bytes\n", RATES_LEN);
		return -1;
	}
	if (host_json_parse_int_in(obj, len, "init_mask", &tmp))
		return -1;
	v->init_mask = (uint16_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect_mask", &tmp))
		return -1;
	v->expect_mask = (uint16_t)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_set_brate_cfg_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		uint16_t cfg = vectors[i].init_mask;

		HalSetBrateCfg(NULL, vectors[i].rates, &cfg);
		if (cfg != vectors[i].expect_mask) {
			fprintf(stderr,
				"FAIL %s: got mask=0x%04x expect=0x%04x\n",
				vectors[i].name, cfg, vectors[i].expect_mask);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
