// SPDX-License-Identifier: GPL-2.0
/* W3-122 L2 C oracle: rtw_mi_status_by_ifbmp + aux wrap/merge/check (#462). */
#include <stdio.h>
#include <string.h>
#include "host_mi_status_types.h"
#include "host_vector_json.h"

static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

struct vector {
	char name[48];
	char op[16];
	int bmp, self_id, chk, expect;
	int f0, f1, f2, f3, a0, a1, s0, mg0, ro0;
	int a_sta, a_ld, a_ap, a_ld_ap, a_scan, a_scan_ent, a_wps;
	int b_sta, b_ld, b_ap, b_ld_ap, b_scan, b_scan_ent, b_wps;
	int e_sta, e_ld, e_lg, e_ap, e_ld_ap, e_start_ap;
	int e_scan, e_scan_ent, e_wps, e_mgmt, e_roch;
	int ld_sta, lg_sta, sta, ap, uwps, ld_mesh;
};

static void setup_iface(int idx, int fw, int asoc, int scan_st, int mgmt, int roch)
{
	struct _adapter *a = &g_if[idx];

	a->mlmepriv.fw_state = fw;
	a->stapriv.asoc_sta_count = asoc;
	a->mlmeextpriv.sitesurvey_res.state = (u8)scan_st;
	g_dv.cfg80211_mgmt_tx[idx] = mgmt ? 1 : 0;
	g_dv.cfg80211_roch[idx] = roch ? 1 : 0;
}

static void reset_adapters(void)
{
	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_dv.padapters[i] = &g_if[i];
	}
}

static int mstate_eq(const struct mi_state *m, const struct vector *v)
{
	return MSTATE_STA_NUM(m) == (u8)v->e_sta &&
	       MSTATE_STA_LD_NUM(m) == (u8)v->e_ld &&
	       MSTATE_STA_LG_NUM(m) == (u8)v->e_lg &&
	       MSTATE_AP_NUM(m) == (u8)v->e_ap &&
	       MSTATE_AP_LD_NUM(m) == (u8)v->e_ld_ap &&
	       MSTATE_AP_STARTING_NUM(m) == (u8)v->e_start_ap &&
	       MSTATE_SCAN_NUM(m) == (u8)v->e_scan &&
	       MSTATE_SCAN_ENTER_NUM(m) == (u8)v->e_scan_ent &&
	       MSTATE_WPS_NUM(m) == (u8)v->e_wps &&
	       MSTATE_MGMT_TX_NUM(m) == (u8)v->e_mgmt &&
	       MSTATE_ROCH_NUM(m) == (u8)v->e_roch;
}

static void fill_mi(struct mi_state *m, const struct vector *v, int side)
{
	memset(m, 0, sizeof(*m));
	if (side == 0) {
		m->sta_num = (u8)v->a_sta;
		m->ld_sta_num = (u8)v->a_ld;
		m->ap_num = (u8)v->a_ap;
		m->ld_ap_num = (u8)v->a_ld_ap;
		m->scan_num = (u8)v->a_scan;
		m->scan_enter_num = (u8)v->a_scan_ent;
		m->uwps_num = (u8)v->a_wps;
	} else {
		m->sta_num = (u8)v->b_sta;
		m->ld_sta_num = (u8)v->b_ld;
		m->ap_num = (u8)v->b_ap;
		m->ld_ap_num = (u8)v->b_ld_ap;
		m->scan_num = (u8)v->b_scan;
		m->scan_enter_num = (u8)v->b_scan_ent;
		m->uwps_num = (u8)v->b_wps;
	}
}

#ifdef HOST_MI_STATUS_RUST
/* Rust host L2 only implements rtw_mi_status_by_ifbmp (#1090); aux/merge/check are C-oracle. */
static int rust_skips_vector(const struct vector *v)
{
	return v->op[0] != '\0';
}
#endif

static int run_vector(struct vector *v)
{
	struct mi_state m, a, b;
	struct _adapter *self = &g_if[v->self_id >= 0 ? v->self_id : 0];

#ifdef HOST_MI_STATUS_RUST
	if (rust_skips_vector(v)) {
		printf("SKIP %s (no Rust port for op=%s)\n", v->name, v->op);
		return 0;
	}
#endif

	if (!strcmp(v->op, "merge")) {
		fill_mi(&a, v, 0);
		fill_mi(&b, v, 1);
		memcpy(&m, &a, sizeof(m));
		rtw_mi_status_merge(&m, &b);
		return mstate_eq(&m, v) ? 0 : -1;
	}
	if (!strcmp(v->op, "check")) {
		reset_adapters();
		g_dv.iface_state.ld_sta_num = (u8)v->ld_sta;
		g_dv.iface_state.lg_sta_num = (u8)v->lg_sta;
		g_dv.iface_state.sta_num = (u8)v->sta;
		g_dv.iface_state.ap_num = (u8)v->ap;
		g_dv.iface_state.uwps_num = (u8)v->uwps;
		g_dv.iface_state.ld_mesh_num = (u8)v->ld_mesh;
		return rtw_mi_check_status(self, (u8)v->chk) == (u8)v->expect ? 0 : -1;
	}

	reset_adapters();
	setup_iface(0, v->f0, v->a0, v->s0, v->mg0, v->ro0);
	setup_iface(1, v->f1, v->a1, 0, 0, 0);
	setup_iface(2, v->f2, 0, 0, 0, 0);
	setup_iface(3, v->f3, 0, 0, 0, 0);

	if (!strcmp(v->op, "status"))
		rtw_mi_status(self, &m);
	else if (!strcmp(v->op, "no_self"))
		rtw_mi_status_no_self(self, &m);
	else if (!strcmp(v->op, "no_others"))
		rtw_mi_status_no_others(self, &m);
	else if (!strcmp(v->op, "update")) {
		rtw_mi_update_iface_status(&self->mlmepriv, WIFI_STATION_STATE);
		return mstate_eq(&g_dv.iface_state, v) ? 0 : -1;
	} else {
		rtw_mi_status_by_ifbmp(&g_dv, (u8)(v->bmp ? v->bmp : 0xFF), &m);
	}
	return mstate_eq(&m, v) ? 0 : -1;
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"op", "bmp", "self_id", "chk", "expect", "f0", "f1", "f2", "f3",
		"a0", "a1", "s0", "mg0", "ro0", "a_sta", "a_ld", "a_ap", "a_ld_ap",
		"a_scan", "a_scan_ent", "a_wps", "b_sta", "b_ld", "b_ap", "b_ld_ap",
		"b_scan", "b_scan_ent", "b_wps", "e_sta", "e_ld", "e_lg", "e_ap",
		"e_ld_ap", "e_start_ap", "e_scan", "e_scan_ent", "e_wps", "e_mgmt",
		"e_roch", "ld_sta", "lg_sta", "sta", "ap", "uwps", "ld_mesh", NULL
	};
	int *vals[] = {
		NULL, &v->bmp, &v->self_id, &v->chk, &v->expect, &v->f0, &v->f1,
		&v->f2, &v->f3, &v->a0, &v->a1, &v->s0, &v->mg0, &v->ro0,
		&v->a_sta, &v->a_ld, &v->a_ap, &v->a_ld_ap, &v->a_scan, &v->a_scan_ent,
		&v->a_wps, &v->b_sta, &v->b_ld, &v->b_ap, &v->b_ld_ap, &v->b_scan,
		&v->b_scan_ent, &v->b_wps, &v->e_sta, &v->e_ld, &v->e_lg, &v->e_ap,
		&v->e_ld_ap, &v->e_start_ap, &v->e_scan, &v->e_scan_ent, &v->e_wps,
		&v->e_mgmt, &v->e_roch, &v->ld_sta, &v->lg_sta, &v->sta, &v->ap,
		&v->uwps, &v->ld_mesh
	};

	memset(v, 0, sizeof(*v));
	v->self_id = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	for (int i = 1; keys[i]; i++)
		host_json_parse_int_in(obj, len, keys[i], vals[i]);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_status_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vector_object, &n))
		return 2;
	for (size_t i = 0; i < n; i++) {
		if (run_vector(&vecs[i]) != 0) {
			fprintf(stderr, "FAIL %s\n", vecs[i].name);
			fail++;
		} else {
			printf("PASS %s\n", vecs[i].name);
		}
	}
	if (fail)
		fprintf(stderr, "FAIL %d/%zu (%s)\n", fail, n, path);
	else
		printf("PASS %zu vectors (%s)\n", n, path);
	return fail ? 1 : 0;
}
