// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_odm_phydm_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	int op;
	int ability;
	int expect;
	int expect_rf;
	int chip_type;
	int expect_ic;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "op", &v->op);
	host_json_parse_int_in(obj, len, "ability", &v->ability);
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	host_json_parse_int_in(obj, len, "expect_rf", &v->expect_rf);
	host_json_parse_int_in(obj, len, "chip_type", &v->chip_type);
	host_json_parse_int_in(obj, len, "expect_ic", &v->expect_ic);
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	u32 got;

	memset(&adapter, 0, sizeof(adapter));
	host_odm_reset(&adapter);

	if (v->op == 100) {
		adapter.chip_type = (u8)v->chip_type;
		rtw_odm_init_ic_type(&adapter);
		got = host_odm_ic_type(adapter_to_phydm(&adapter));
		if (got != (u32)v->expect_ic)
			goto fail;
	} else if (v->op == 0) {
		rtw_phydm_ability_ops(&adapter, HAL_PHYDM_FUNC_SET, (u32)v->ability);
		got = rtw_phydm_ability_ops(&adapter, HAL_PHYDM_ABILITY_GET, 0);
		if (got != (u32)v->expect)
			goto fail;
	} else if (v->op == 2) {
		rtw_phydm_ability_ops(&adapter, HAL_PHYDM_DIS_ALL_FUNC, 0);
		got = rtw_phydm_ability_ops(&adapter, HAL_PHYDM_ABILITY_GET, 0);
		if (got != (u32)v->expect ||
		    host_odm_rf_ability(adapter_to_phydm(&adapter)) != (u32)v->expect_rf)
			goto fail;
	} else {
		goto fail;
	}
	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return -1;
}
int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t nvec = 0;
	int failed = 0;
	const char *path = (argc > 1) ? argv[1] : "odm_phydm_init_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &nvec))
		return 2;
	for (size_t i = 0; i < nvec; i++)
		failed += run_vector(&vectors[i]) != 0;
	if (!failed)
		printf("PASS %zu vectors (%s)\n", nvec, path);
	return failed ? 1 : 0;
}
