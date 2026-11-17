// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_ra_registed_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	int sta_null;
	int macid_ctl_num;
	int mac_id;
	int rssi;
	int expect_warn_on;
	int expect_err;
	int expect_phydm_calls;
	int expect_mac_id;
	int expect_rssi;
	int expect_dump;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "sta_null", &v->sta_null))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "macid_ctl_num", &ul))
		return -1;
	v->macid_ctl_num = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "mac_id", &ul))
		return -1;
	v->mac_id = (int)ul;
	if (host_json_parse_int_in(obj, len, "rssi", &v->rssi))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_warn_on", &v->expect_warn_on))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_err", &v->expect_err))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_phydm_calls",
				   &v->expect_phydm_calls))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_mac_id", &ul)) {
		v->expect_mac_id = -1;
	} else {
		v->expect_mac_id = (int)ul;
	}
	if (host_json_parse_int_in(obj, len, "expect_rssi", &v->expect_rssi))
		v->expect_rssi = -1;
	if (host_json_parse_int_in(obj, len, "expect_dump", &v->expect_dump))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;
	struct host_hal_dm_phydm_ra_registed_trace *t =
		&host_hal_dm_phydm_ra_registed_trace;
	struct sta_info *psta = v->sta_null ? NULL : &sta;

	host_hal_dm_phydm_ra_registed_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.dvobj.macid_ctl.num = (u8)v->macid_ctl_num;
	sta.cmn.mac_id = (u8)v->mac_id;
	sta.cmn.rssi_stat.rssi = (s8)v->rssi;

	rtw_phydm_ra_registed(&adapter, psta);

	if (t->warn_on_calls != (u8)v->expect_warn_on)
		return -1;
	if (t->err_logged != (u8)v->expect_err)
		return -1;
	if (t->phydm_ra_calls != (u8)v->expect_phydm_calls)
		return -1;
	if (v->expect_phydm_calls && t->last_mac_id != (u8)v->expect_mac_id)
		return -1;
	if (v->expect_phydm_calls && t->last_rssi != (u8)v->expect_rssi)
		return -1;
	if (t->dump_sta_calls != (u8)v->expect_dump)
		return -1;

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_phydm_ra_registed_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}