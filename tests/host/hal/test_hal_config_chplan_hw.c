// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for hal_com_config_channel_plan HW branch (W4-90). */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/host_hal_config_chplan_hw_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 64

enum alpha2_mode {
	ALPHA2_NULL = 0,
	ALPHA2_PTR,
	ALPHA2_UNSPECIFIED,
};

struct vector {
	char name[MAX_NAME];
	enum alpha2_mode alpha2_mode;
	char hw_alpha2[3];
	uint8_t hw_chplan;
	uint8_t autoload_fail;
	int expect_chplan;
	uint8_t expect_country_chplan;
	uint8_t expect_force_hw;
	uint8_t expect_goto_sw;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;
	char alpha[8];

	memset(v, 0, sizeof(*v));
	v->alpha2_mode = ALPHA2_NULL;
	v->expect_country_chplan = HAL_CFG_CHPLAN_NO_COUNTRY;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_string_in(obj, len, "hw_alpha2", alpha, sizeof(alpha))) {
		if (alpha[0] == '\xff' && alpha[1] == '\xff' && alpha[2] == '\0') {
			v->alpha2_mode = ALPHA2_UNSPECIFIED;
			v->hw_alpha2[0] = '\xff';
			v->hw_alpha2[1] = '\xff';
		} else if (alpha[0]) {
			v->alpha2_mode = ALPHA2_PTR;
			v->hw_alpha2[0] = alpha[0];
			v->hw_alpha2[1] = alpha[1];
		}
	}
	if (host_json_parse_int_in(obj, len, "hw_chplan", &tmp))
		return -1;
	v->hw_chplan = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "autoload_fail", &tmp))
		v->autoload_fail = 0;
	else
		v->autoload_fail = (uint8_t)tmp;
	if (host_json_parse_int_in(obj, len, "expect_chplan", &v->expect_chplan))
		return -1;
	if (!host_json_parse_int_in(obj, len, "expect_country_chplan", &tmp))
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
		const char *alpha2 = NULL;

		if (vectors[i].alpha2_mode == ALPHA2_PTR ||
		    vectors[i].alpha2_mode == ALPHA2_UNSPECIFIED)
			alpha2 = vectors[i].hw_alpha2;

		hal_com_config_chplan_hw_branch(alpha2, vectors[i].hw_chplan,
						vectors[i].autoload_fail, &got);
		if (got.chplan != vectors[i].expect_chplan ||
		    got.country_chplan != vectors[i].expect_country_chplan ||
		    got.force_hw_chplan != vectors[i].expect_force_hw ||
		    got.early_goto_sw_config != vectors[i].expect_goto_sw) {
			fprintf(stderr, "FAIL %s chplan=%d force=%u goto=%u\n",
				vectors[i].name, got.chplan, got.force_hw_chplan,
				got.early_goto_sw_config);
			return 1;
		}
	}

	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}