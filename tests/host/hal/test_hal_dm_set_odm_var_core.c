// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_set_odm_var_core_types.h"
#include "host_vector_json.h"


struct host_hal_dm_set_odm_var_core_trace host_hal_dm_set_odm_var_core_trace;

void host_hal_dm_set_odm_var_core_reset(void)
{
	memset(&host_hal_dm_set_odm_var_core_trace, 0,
	       sizeof(host_hal_dm_set_odm_var_core_trace));
}

void host_hal_dm_set_odm_var_core_sta_log_set(u8 mac_id)
{
	host_hal_dm_set_odm_var_core_trace.sta_log_set = 1;
	(void)mac_id;
}

void host_hal_dm_set_odm_var_core_sta_log_clean(u8 mac_id)
{
	host_hal_dm_set_odm_var_core_trace.sta_log_clean = 1;
	(void)mac_id;
}

void phydm_cmn_sta_info_hook(struct dm_struct *dm, u8 mac_id, void *sta)
{
	(void)dm;
	host_hal_dm_set_odm_var_core_trace.sta_hook_macid = mac_id;
	host_hal_dm_set_odm_var_core_trace.sta_hook_null = sta ? 0 : 1;
}

void odm_cmn_info_update(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	host_hal_dm_set_odm_var_core_trace.cmn_update_last = cmn_info;
	host_hal_dm_set_odm_var_core_trace.cmn_update_value = value;
}

void phydm_adaptivity_info_update(void *dm_void, u32 cmn_info, u64 value)
{
	(void)dm_void;
	host_hal_dm_set_odm_var_core_trace.adapt_update_last = cmn_info;
	host_hal_dm_set_odm_var_core_trace.adapt_update_value = value;
	host_hal_dm_set_odm_var_core_trace.adapt_call_count++;
}

void odm_pause_dig(void *dm_void, u8 pause_type, u8 pause_level, u8 igi)
{
	(void)dm_void;
	host_hal_dm_set_odm_var_core_trace.pause_type = pause_type;
	host_hal_dm_set_odm_var_core_trace.pause_level = pause_level;
	host_hal_dm_set_odm_var_core_trace.pause_igi = igi;
}

struct vector {
	char name[48];
	char case_name[24];
	int sta_mac_id;
	int bset;
	int rx_gain;
	int expect_dm_ctrl;
	int expect_sta_hook_macid;
	int expect_sta_hook_null;
	int expect_sta_log_set;
	int expect_sta_log_clean;
	int expect_cmn_info;
	int expect_cmn_value;
	int expect_adapt_count;
	int expect_pause_type;
	int expect_pause_igi;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"sta_mac_id", "bset", "rx_gain", "expect_dm_ctrl",
		"expect_sta_hook_macid", "expect_sta_hook_null",
		"expect_sta_log_set", "expect_sta_log_clean",
		"expect_cmn_info", "expect_cmn_value", "expect_adapt_count",
		"expect_pause_type", "expect_pause_igi",
	};
	int *dst[] = {
		&v->sta_mac_id, &v->bset, &v->rx_gain, &v->expect_dm_ctrl,
		&v->expect_sta_hook_macid, &v->expect_sta_hook_null,
		&v->expect_sta_log_set, &v->expect_sta_log_clean,
		&v->expect_cmn_info, &v->expect_cmn_value, &v->expect_adapt_count,
		&v->expect_pause_type, &v->expect_pause_igi,
	};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "case", v->case_name,
				    sizeof(v->case_name)))
		return -1;
	for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		(void)host_json_parse_int_in(obj, len, keys[i], dst[i]);
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct sta_info psta;
	u8 rx_gain;

	host_hal_dm_set_odm_var_core_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&psta, 0, sizeof(psta));
	psta.cmn.mac_id = (u8)v->sta_mac_id;

	if (!strcmp(v->case_name, "sta")) {
		SetHalODMVar(&adapter, HAL_ODM_STA_INFO, &psta, (BOOLEAN)v->bset);
		if (psta.cmn.dm_ctrl != (u8)v->expect_dm_ctrl)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.sta_hook_macid !=
		    (u8)v->expect_sta_hook_macid)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.sta_hook_null !=
		    (u8)v->expect_sta_hook_null)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.sta_log_set !=
		    (u8)v->expect_sta_log_set)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.sta_log_clean !=
		    (u8)v->expect_sta_log_clean)
			return -1;
	} else if (!strcmp(v->case_name, "p2p")) {
		SetHalODMVar(&adapter, HAL_ODM_P2P_STATE, NULL,
			     (BOOLEAN)v->bset);
		if (host_hal_dm_set_odm_var_core_trace.cmn_update_last !=
		    (u32)v->expect_cmn_info)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.cmn_update_value !=
		    (u64)v->expect_cmn_value)
			return -1;
	} else if (!strcmp(v->case_name, "wfd")) {
		SetHalODMVar(&adapter, HAL_ODM_WIFI_DISPLAY_STATE, NULL,
			     (BOOLEAN)v->bset);
		if (host_hal_dm_set_odm_var_core_trace.cmn_update_last !=
		    (u32)v->expect_cmn_info)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.cmn_update_value !=
		    (u64)v->expect_cmn_value)
			return -1;
	} else if (!strcmp(v->case_name, "regulation")) {
		SetHalODMVar(&adapter, HAL_ODM_REGULATION, NULL, 0);
		if (host_hal_dm_set_odm_var_core_trace.adapt_call_count !=
		    (u8)v->expect_adapt_count)
			return -1;
	} else if (!strcmp(v->case_name, "initial_gain")) {
		rx_gain = (u8)v->rx_gain;
		SetHalODMVar(&adapter, HAL_ODM_INITIAL_GAIN, &rx_gain, 0);
		if (host_hal_dm_set_odm_var_core_trace.pause_type !=
		    (u8)v->expect_pause_type)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.pause_igi !=
		    (u8)v->expect_pause_igi)
			return -1;
		if (host_hal_dm_set_odm_var_core_trace.pause_level !=
		    PHYDM_PAUSE_LEVEL_0)
			return -1;
	} else {
		return -1;
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[16];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				  : "hal_dm_set_odm_var_core_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}