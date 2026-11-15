// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_igi_iqk_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_igi_iqk_trace host_hal_dm_phydm_igi_iqk_trace;

void host_hal_dm_phydm_igi_iqk_reset(void)
{
	memset(&host_hal_dm_phydm_igi_iqk_trace, 0, sizeof(host_hal_dm_phydm_igi_iqk_trace));
}

void host_hal_dm_phydm_igi_iqk_trace_err(void)
{
	host_hal_dm_phydm_igi_iqk_trace.err_logged = 1;
}

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type)
{
	(void)dm;
	host_hal_dm_phydm_igi_iqk_trace.phydm_cmn_query_last = info_type;
	return host_hal_dm_phydm_igi_iqk_trace.phydm_cmn_query_ret;
}

void odm_acquire_spin_lock(struct dm_struct *dm, u32 lock_type)
{
	(void)dm;
	if (lock_type == RT_IQK_SPINLOCK)
		host_hal_dm_phydm_igi_iqk_trace.iqk_lock_acquired = 1;
}

void odm_release_spin_lock(struct dm_struct *dm, u32 lock_type)
{
	(void)dm;
	(void)lock_type;
}

struct vector {
	char name[48];
	char op[8];
	int fixture_igi;
	int expect_igi;
	int fixture_iqk;
	int expect_iqk;
	int expect_iqk_lock;
	int expect_iqk_err;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"fixture_igi", "expect_igi", "fixture_iqk", "expect_iqk",
		"expect_iqk_lock", "expect_iqk_err",
	};
	int *dst[] = {
		&v->fixture_igi, &v->expect_igi, &v->fixture_iqk, &v->expect_iqk,
		&v->expect_iqk_lock, &v->expect_iqk_err,
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

static void fill_adapter(struct vector *v, _adapter *adapter)
{
	memset(adapter, 0, sizeof(*adapter));
	GET_HAL_DATA(adapter)->odmpriv.rf_calibrate_info.is_iqk_in_progress =
		(u8)v->fixture_iqk;
	host_hal_dm_phydm_igi_iqk_trace.phydm_cmn_query_ret = (u32)v->fixture_igi;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	u8 got;

	host_hal_dm_phydm_igi_iqk_reset();
	fill_adapter(v, &adapter);
	if (!strcmp(v->op, "igi")) {
		got = rtw_phydm_get_cur_igi(&adapter);
		if (got != (u8)v->expect_igi)
			return -1;
		if (host_hal_dm_phydm_igi_iqk_trace.phydm_cmn_query_last != PHYDM_INFO_CURR_IGI)
			return -1;
	} else if (!strcmp(v->op, "iqk")) {
		got = rtw_phydm_is_iqk_in_progress(&adapter);
		if (got != (u8)v->expect_iqk)
			return -1;
		if (!!host_hal_dm_phydm_igi_iqk_trace.iqk_lock_acquired !=
		    !!v->expect_iqk_lock)
			return -1;
		if (!!host_hal_dm_phydm_igi_iqk_trace.err_logged != !!v->expect_iqk_err)
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
	const char *path = argc > 1 ? argv[1] : "hal_dm_phydm_igi_iqk_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8, parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	printf("OK %zu vectors\n", count);
	return 0;
}