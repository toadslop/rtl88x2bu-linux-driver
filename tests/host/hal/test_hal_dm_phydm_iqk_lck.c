// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_iqk_lck_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_iqk_lck_trace host_hal_dm_phydm_iqk_lck_trace;

void host_hal_dm_phydm_iqk_lck_reset(void)
{
	memset(&host_hal_dm_phydm_iqk_lck_trace, 0,
	       sizeof(host_hal_dm_phydm_iqk_lck_trace));
}

void halrf_cmn_info_set(void *dm_void, u32 cmn_info, u64 value)
{
	u8 idx = host_hal_dm_phydm_iqk_lck_trace.halrf_cmn_set_count;

	(void)dm_void;
	if (idx < 8) {
		host_hal_dm_phydm_iqk_lck_trace.halrf_cmn_set_info[idx] = cmn_info;
		host_hal_dm_phydm_iqk_lck_trace.halrf_cmn_set_value[idx] = value;
	}
	host_hal_dm_phydm_iqk_lck_trace.halrf_cmn_set_count++;
}

void halrf_segment_iqk_trigger(void *dm_void, boolean clear, boolean segment)
{
	(void)dm_void;
	host_hal_dm_phydm_iqk_lck_trace.segment_clear = clear;
	host_hal_dm_phydm_iqk_lck_trace.segment_flag = segment;
	host_hal_dm_phydm_iqk_lck_trace.segment_calls++;
}

void halrf_iqk_trigger(void *dm_void, boolean is_recovery)
{
	(void)dm_void;
	host_hal_dm_phydm_iqk_lck_trace.plain_iqk_recovery = is_recovery;
	host_hal_dm_phydm_iqk_lck_trace.plain_iqk_calls++;
}

void halrf_lck_trigger(void *dm_void)
{
	(void)dm_void;
	host_hal_dm_phydm_iqk_lck_trace.lck_calls++;
}

struct vector {
	char name[48];
	char op[16];
	int expect_cmn_sets;
	int expect_rfk_forbidden;
	int expect_iqk_segment;
	int expect_segment;
	int expect_segment_clear;
	int expect_segment_flag;
	int expect_lck;
	int expect_plain_iqk;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"expect_cmn_sets", "expect_rfk_forbidden", "expect_iqk_segment",
		"expect_segment", "expect_segment_clear", "expect_segment_flag",
		"expect_lck", "expect_plain_iqk",
	};
	int *dst[] = {
		&v->expect_cmn_sets, &v->expect_rfk_forbidden, &v->expect_iqk_segment,
		&v->expect_segment, &v->expect_segment_clear, &v->expect_segment_flag,
		&v->expect_lck, &v->expect_plain_iqk,
	};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		if (host_json_parse_int_in(obj, len, keys[i], dst[i]))
			return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_iqk_lck_trace *t = &host_hal_dm_phydm_iqk_lck_trace;

	host_hal_dm_phydm_iqk_lck_reset();
	memset(&adapter, 0, sizeof(adapter));

	if (!strcmp(v->op, "iqk_trigger"))
		rtw_phydm_iqk_trigger(&adapter);
	else if (!strcmp(v->op, "iqk_trigger_all"))
		rtw_phydm_iqk_trigger_all(&adapter);
	else if (!strcmp(v->op, "lck_trigger"))
		rtw_phydm_lck_trigger(&adapter);
	else
		return -1;

	if (t->halrf_cmn_set_count != (u8)v->expect_cmn_sets)
		return -1;
	if (v->expect_cmn_sets >= 1 &&
	    t->halrf_cmn_set_info[0] != HALRF_CMNINFO_RFK_FORBIDDEN)
		return -1;
	if (v->expect_cmn_sets >= 1 &&
	    t->halrf_cmn_set_value[0] != (u64)v->expect_rfk_forbidden)
		return -1;
	if (v->expect_cmn_sets >= 2 &&
	    t->halrf_cmn_set_info[1] != HALRF_CMNINFO_IQK_SEGMENT)
		return -1;
	if (v->expect_cmn_sets >= 2 &&
	    t->halrf_cmn_set_value[1] != (u64)v->expect_iqk_segment)
		return -1;
	if (t->segment_calls != (u8)v->expect_segment)
		return -1;
	if (!!t->segment_clear != !!v->expect_segment_clear)
		return -1;
	if (!!t->segment_flag != !!v->expect_segment_flag)
		return -1;
	if (t->lck_calls != (u8)v->expect_lck)
		return -1;
	if (t->plain_iqk_calls != (u8)v->expect_plain_iqk)
		return -1;

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_phydm_iqk_lck_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}