// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_halmac_dm_leaf_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	char op[16];
	int gpio;
	int clear;
	int segment;
	int rfe_stub_return;
	int iqk_stub_return;
	int expect_bool;
	int expect_status;
	int expect_rfe_calls;
	int expect_gpio;
	int expect_iqk_calls;
	int expect_clear;
	int expect_segment;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "gpio", &ul))
		return -1;
	v->gpio = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "clear", &ul))
		return -1;
	v->clear = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "segment", &ul))
		return -1;
	v->segment = (int)ul;
	if (host_json_parse_int_in(obj, len, "rfe_stub_return", &v->rfe_stub_return))
		return -1;
	if (host_json_parse_int_in(obj, len, "iqk_stub_return", &v->iqk_stub_return))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_bool", &v->expect_bool))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_status", &v->expect_status))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_rfe_calls", &v->expect_rfe_calls))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_gpio", &ul))
		return -1;
	v->expect_gpio = (int)ul;
	if (host_json_parse_int_in(obj, len, "expect_iqk_calls", &v->expect_iqk_calls))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_clear", &ul))
		return -1;
	v->expect_clear = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "expect_segment", &ul))
		return -1;
	v->expect_segment = (int)ul;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct dm_struct phydm;
	struct host_hal_dm_halmac_dm_leaf_trace *t =
		&host_hal_dm_halmac_dm_leaf_trace;
	boolean ret_bool;
	enum hal_status ret_status;

	host_hal_dm_halmac_dm_leaf_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&phydm, 0, sizeof(phydm));
	phydm.adapter = &adapter;
	t->rfe_stub_return = (s8)v->rfe_stub_return;
	t->iqk_stub_return = (s8)v->iqk_stub_return;

	if (!strcmp(v->op, "rfe_gpio")) {
		ret_bool = rtw_phydm_rfe_ctrl_gpio(&adapter, (u8)v->gpio);
		if (ret_bool != (boolean)v->expect_bool)
			return -1;
		if (t->rfe_calls != (u8)v->expect_rfe_calls)
			return -1;
		if (t->last_gpio != (u8)v->expect_gpio)
			return -1;
	} else if (!strcmp(v->op, "fw_iqk")) {
		ret_status = rtw_phydm_fw_iqk(&phydm, (u8)v->clear, (u8)v->segment);
		if ((int)ret_status != v->expect_status)
			return -1;
		if (t->iqk_calls != (u8)v->expect_iqk_calls)
			return -1;
		if (t->last_clear != (u8)v->expect_clear)
			return -1;
		if (t->last_segment != (u8)v->expect_segment)
			return -1;
	} else {
		return -1;
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_halmac_dm_leaf_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}