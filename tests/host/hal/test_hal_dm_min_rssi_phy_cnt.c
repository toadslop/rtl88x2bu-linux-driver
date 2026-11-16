// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_min_rssi_phy_cnt_types.h"
#include "host_vector_json.h"

struct host_hal_dm_min_rssi_phy_cnt_trace host_hal_dm_min_rssi_phy_cnt_trace;

static struct sta_info sta_pool[4];

void host_hal_dm_min_rssi_phy_cnt_reset(void)
{
	memset(&host_hal_dm_min_rssi_phy_cnt_trace, 0,
	       sizeof(host_hal_dm_min_rssi_phy_cnt_trace));
}

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type)
{
	(void)dm;
	host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_last = info_type;
	return host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_ret;
}

struct vector {
	char name[48];
	char op[24];
	int sta0_present;
	int sta0_rssi;
	int sta0_opmode;
	int sta0_bcast;
	int sta1_present;
	int sta1_rssi;
	int sta1_opmode;
	int sta1_bcast;
	int sta2_present;
	int sta2_rssi;
	int sta2_opmode;
	int sta2_bcast;
	int fixture_query_ret;
	int expect_s8;
	int expect_u8;
	int expect_u32;
	int expect_query_type;
	int phy_cnt;
};

static void apply_sta_slot(_adapter *adapter, int slot, int present, int rssi,
			   int opmode, int bcast)
{
	struct macid_ctl_t *ctl = adapter_to_macidctl(adapter);

	if (!present) {
		ctl->sta[slot] = NULL;
		return;
	}
	memset(&sta_pool[slot], 0, sizeof(sta_pool[slot]));
	if (bcast)
		memset(sta_pool[slot].cmn.mac_addr, 0xff, 6);
	else
		sta_pool[slot].cmn.mac_addr[0] = (u8)(slot + 1);
	sta_pool[slot].cmn.rssi_stat.rssi = (s8)rssi;
	ctl->sta[slot] = &sta_pool[slot];
	ctl->h2c_msr[slot] = (u8)(opmode & 1);
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"sta0_present", "sta0_rssi", "sta0_opmode", "sta0_bcast",
		"sta1_present", "sta1_rssi", "sta1_opmode", "sta1_bcast",
		"sta2_present", "sta2_rssi", "sta2_opmode", "sta2_bcast",
		"fixture_query_ret", "expect_s8", "expect_u8", "expect_u32",
		"expect_query_type", "phy_cnt",
	};
	int *dst[] = {
		&v->sta0_present, &v->sta0_rssi, &v->sta0_opmode, &v->sta0_bcast,
		&v->sta1_present, &v->sta1_rssi, &v->sta1_opmode, &v->sta1_bcast,
		&v->sta2_present, &v->sta2_rssi, &v->sta2_opmode, &v->sta2_bcast,
		&v->fixture_query_ret, &v->expect_s8, &v->expect_u8, &v->expect_u32,
		&v->expect_query_type, &v->phy_cnt,
	};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		(void)host_json_parse_int_in(obj, len, keys[i], dst[i]);
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;

	host_hal_dm_min_rssi_phy_cnt_reset();
	memset(&adapter, 0, sizeof(adapter));
	host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_ret =
		(u32)v->fixture_query_ret;

	apply_sta_slot(&adapter, 0, v->sta0_present, v->sta0_rssi, v->sta0_opmode,
		       v->sta0_bcast);
	apply_sta_slot(&adapter, 1, v->sta1_present, v->sta1_rssi, v->sta1_opmode,
		       v->sta1_bcast);
	apply_sta_slot(&adapter, 2, v->sta2_present, v->sta2_rssi, v->sta2_opmode,
		       v->sta2_bcast);

	if (!strcmp(v->op, "dm_min_rssi")) {
		s8 got = rtw_dm_get_min_rssi(&adapter);
		if (got != (s8)v->expect_s8)
			return -1;
	} else if (!strcmp(v->op, "phydm_min_rssi")) {
		s8 got = rtw_phydm_get_min_rssi(&adapter);
		if (got != (s8)v->expect_s8)
			return -1;
		if (host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_last !=
		    (u32)v->expect_query_type)
			return -1;
	} else if (!strcmp(v->op, "edcca")) {
		u8 got = (u8)rtw_phydm_get_edcca_flag(&adapter);
		if (got != (u8)v->expect_u8)
			return -1;
		if (host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_last !=
		    (u32)v->expect_query_type)
			return -1;
	} else if (!strcmp(v->op, "phy_cnt")) {
		u32 got = rtw_phydm_get_phy_cnt(&adapter, (enum phy_cnt)v->phy_cnt);
		if (got != (u32)v->expect_u32)
			return -1;
		if (v->expect_query_type &&
		    host_hal_dm_min_rssi_phy_cnt_trace.phydm_cmn_query_last !=
			    (u32)v->expect_query_type)
			return -1;
	} else {
		return -1;
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[12];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_dm_min_rssi_phy_cnt_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 12,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	printf("OK %zu vectors\n", count);
	return 0;
}