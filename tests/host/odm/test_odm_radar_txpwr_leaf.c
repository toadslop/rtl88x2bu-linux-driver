// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_odm_radar_txpwr_types.h"
#include "host_vector_json.h"

struct host_odm_radar_trace host_odm_radar_trace;

void host_odm_radar_txpwr_reset(_adapter *a, struct dvobj_priv *d)
{
	memset(&host_odm_radar_trace, 0, sizeof(host_odm_radar_trace));
	memset(a, 0, sizeof(*a));
	memset(d, 0, sizeof(*d));
	a->dvobj = d;
	GET_HAL_DATA(a)->odmpriv.adapter = a;
}

u8 rtw_rfctl_get_dfs_domain(struct rf_ctl_t *r) { return r->dfs_region_domain; }
void odm_cmn_info_init(struct dm_struct *dm, u32 id, u32 val)
{
	if (id == ODM_CMNINFO_DFS_REGION_DOMAIN) {
		dm->dfs_region_domain = val;
		host_odm_radar_trace.dfs_region_init = val;
	}
}
u8 mgn_rate_to_rs(u8 rate) { return rate & 0x7f; }
s16 phy_get_txpwr_single_mbm(_adapter *a, u8 rfpath, u8 rs, u8 rate, u8 bw, u8 cch,
			     u8 o1, u8 o2, u8 o3, void *p)
{
	(void)a; (void)rs; (void)o1; (void)o2; (void)o3; (void)p;
	host_odm_radar_trace.txpwr_calls++;
	return (s16)(100 * rfpath + rate + bw + cch);
}
void phydm_radar_detect_reset(struct dm_struct *dm) { (void)dm; host_odm_radar_trace.radar_reset++; }
void phydm_radar_detect_disable(struct dm_struct *dm) { (void)dm; host_odm_radar_trace.radar_disable++; }
void phydm_radar_detect_enable(struct dm_struct *dm) { (void)dm; host_odm_radar_trace.radar_enable++; }
BOOLEAN phydm_radar_detect(struct dm_struct *dm)
{
	host_odm_radar_trace.radar_detect++;
	return dm->mock_radar_detect ? _TRUE : _FALSE;
}
u8 phydm_dfs_polling_time(struct dm_struct *dm) { return dm->dfs_polling_ms; }

_adapter *host_odm_dm_adapter(struct dm_struct *dm) { return dm->adapter; }
struct dm_struct *host_adapter_to_phydm(_adapter *a) { return adapter_to_phydm(a); }
struct dm_struct *host_dvobj_to_phydm(struct dvobj_priv *d) { return dvobj_to_phydm(d); }
struct rf_ctl_t *host_dvobj_to_rfctl(struct dvobj_priv *d) { return dvobj_to_rfctl(d); }

#ifndef HOST_ODM_RADAR_RUST
#include "../../../core/rtw_odm_radar_txpwr_leaf.c"
#endif

struct vector {
	char name[48], op[24];
	int rfpath, rate, bw, cch, expect_txpwr;
	int expect_radar_reset, mock_radar_detect, expect_radar_detect_ret, expect_radar_detect_calls;
	int dfs_region, expect_dfs_init, dfs_polling_ms, expect_polling;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "rfpath", &v->rfpath);
	host_json_parse_int_in(obj, len, "rate", &v->rate);
	host_json_parse_int_in(obj, len, "bw", &v->bw);
	host_json_parse_int_in(obj, len, "cch", &v->cch);
	host_json_parse_int_in(obj, len, "expect_txpwr", &v->expect_txpwr);
	host_json_parse_int_in(obj, len, "expect_radar_reset", &v->expect_radar_reset);
	host_json_parse_int_in(obj, len, "mock_radar_detect", &v->mock_radar_detect);
	host_json_parse_int_in(obj, len, "expect_radar_detect_ret", &v->expect_radar_detect_ret);
	host_json_parse_int_in(obj, len, "expect_radar_detect_calls", &v->expect_radar_detect_calls);
	host_json_parse_int_in(obj, len, "dfs_region", &v->dfs_region);
	host_json_parse_int_in(obj, len, "expect_dfs_init", &v->expect_dfs_init);
	host_json_parse_int_in(obj, len, "dfs_polling_ms", &v->dfs_polling_ms);
	host_json_parse_int_in(obj, len, "expect_polling", &v->expect_polling);
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct dvobj_priv dvobj;
	struct dm_struct *odm;

	host_odm_radar_txpwr_reset(&adapter, &dvobj);
	odm = adapter_to_phydm(&adapter);
	odm->mock_radar_detect = (u8)v->mock_radar_detect;
	odm->dfs_polling_ms = (u8)v->dfs_polling_ms;
	dvobj.rfctl.dfs_region_domain = (u8)v->dfs_region;
	dvobj.hal.odmpriv = *odm;

	if (!strcmp(v->op, "txpwr")) {
		if (rtw_odm_get_tx_power_mbm(odm, (u8)v->rfpath, (u8)v->rate, (u8)v->bw, (u8)v->cch) != v->expect_txpwr)
			goto fail;
	} else if (!strcmp(v->op, "radar_reset")) {
		rtw_odm_radar_detect_reset(&adapter);
		if (v->expect_radar_reset && host_odm_radar_trace.radar_reset != v->expect_radar_reset)
			goto fail;
	} else if (!strcmp(v->op, "radar_detect")) {
		if ((int)rtw_odm_radar_detect(&adapter) != v->expect_radar_detect_ret)
			goto fail;
		if (v->expect_radar_detect_calls && host_odm_radar_trace.radar_detect != v->expect_radar_detect_calls)
			goto fail;
	} else if (!strcmp(v->op, "update_dfs")) {
		rtw_odm_update_dfs_region(&dvobj);
		if (v->expect_dfs_init && (int)host_odm_radar_trace.dfs_region_init != v->expect_dfs_init)
			goto fail;
	} else if (!strcmp(v->op, "polling")) {
		if ((int)rtw_odm_radar_detect_polling_int_ms(&dvobj) != v->expect_polling)
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
	struct vector vectors[16];
	size_t nvec = 0;
	int failed = 0;
	const char *path = (argc > 1) ? argv[1] : "odm_radar_txpwr_leaf_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16, parse_vector_object, &nvec))
		return 2;
	for (size_t i = 0; i < nvec; i++)
		if (run_vector(&vectors[i]))
			failed++;
	return failed ? 1 : 0;
}
