// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_get_odm_var_types.h"
#include "host_vector_json.h"

struct host_hal_dm_get_odm_var_trace host_hal_dm_get_odm_var_trace;

void host_hal_dm_get_odm_var_reset(void)
{
	memset(&host_hal_dm_get_odm_var_trace, 0, sizeof(host_hal_dm_get_odm_var_trace));
}

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type)
{
	(void)dm;
	host_hal_dm_get_odm_var_trace.phydm_cmn_query_last = info_type;
	return host_hal_dm_get_odm_var_trace.phydm_cmn_query_ret;
}

struct vector {
	char name[48];
	char op[12];
	int fixture_igi;
	int expect_out;
	int fixture_antdiv;
	int expect_query;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"fixture_igi", "expect_out", "fixture_antdiv", "expect_query",
	};
	int *dst[] = { &v->fixture_igi, &v->expect_out, &v->fixture_antdiv,
		       &v->expect_query };

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		if (host_json_parse_int_in(obj, len, keys[i], dst[i]))
			return -1;
	return 0;
}

static void fill_adapter(struct vector *v, _adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	adapter_to_phydm(adapter)->dm_fat_table.rx_idle_ant = (u8)v->fixture_antdiv;
	host_hal_dm_get_odm_var_trace.phydm_cmn_query_ret = (u32)v->fixture_igi;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	u8 out = 111;

	host_hal_dm_get_odm_var_reset();
	fill_adapter(v, &adapter);
	if (!strcmp(v->op, "igi")) {
		GetHalODMVar(&adapter, HAL_ODM_INITIAL_GAIN, &out, NULL);
		if (out != (u8)v->expect_out)
			return -1;
		if (host_hal_dm_get_odm_var_trace.phydm_cmn_query_last !=
		    (u32)v->expect_query)
			return -1;
#ifdef CONFIG_ANTENNA_DIVERSITY
	} else if (!strcmp(v->op, "antdiv")) {
		GetHalODMVar(&adapter, HAL_ODM_ANTDIV_SELECT, &out, NULL);
		if (out != (u8)v->expect_out)
			return -1;
		if (host_hal_dm_get_odm_var_trace.phydm_cmn_query_last != 0)
			return -1;
#endif
	} else if (!strcmp(v->op, "noop")) {
		GetHalODMVar(&adapter, 99, &out, NULL);
		if (out != (u8)v->expect_out)
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
	const char *path = argc > 1 ? argv[1] : "hal_dm_get_odm_var_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8, parse_vector_object,
			      &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}