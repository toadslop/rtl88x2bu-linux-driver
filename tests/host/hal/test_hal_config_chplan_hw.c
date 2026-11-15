// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for hal_com_config_channel_plan HW branch (W4-90). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/host_hal_config_chplan_hw_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char hw_alpha2[3];
	uint8_t has_alpha2;
	uint8_t hw_chplan;
	uint8_t force_hw_in;
	int expect_chplan;
	uint8_t expect_country_chplan;
	uint8_t expect_force_hw;
	uint8_t expect_goto_sw;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;
	char alpha[3];

	memset(v, 0, sizeof(*v));
	v->has_alpha2 = 0;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_string_in(obj, len, "hw_alpha2", alpha, 3) && alpha[0]) {
		v->hw_alpha2[0] = alpha[0];
		v->hw_alpha2[1] = alpha[1];
		v->has_alpha2 = 1;
	}
	if (host_json_parse_int_in(obj, len, "hw_chplan", &tmp))
		return -1;
	v->hw_chplan = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "force_hw_in", &tmp))
		return -1;
	v->force_hw_in = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect_chplan", &v->expect_chplan))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_country_chplan", &tmp))
		return -1;
	v->expect_country_chplan = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect_force_hw", &tmp))
		return -1;
	v->expect_force_hw = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect_goto_sw", &tmp))
		return -1;
	v->expect_goto_sw = (uint8_t)tmp;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] :
		"hal_config_chplan_hw_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < count; i++) {
		struct hal_config_chplan_hw_out got;
		const char *alpha2 = vectors[i].has_alpha2 ? vectors[i].hw_alpha2 : NULL;

		hal_com_config_chplan_hw_branch(alpha2, vectors[i].hw_chplan,
						vectors[i].force_hw_in, &got);
		if (got.chplan != vectors[i].expect_chplan ||
		    got.country_chplan != vectors[i].expect_country_chplan ||
		    got.force_hw_chplan != vectors[i].expect_force_hw ||
		    got.goto_sw_config != vectors[i].expect_goto_sw) {
			fprintf(stderr, "FAIL %s\n", vectors[i].name);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}