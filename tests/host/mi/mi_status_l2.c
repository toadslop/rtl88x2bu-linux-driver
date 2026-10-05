// SPDX-License-Identifier: GPL-2.0
/* W3-122 PR2 L2 C oracle: rtw_mi_status_by_ifbmp (core/rtw_mi_status.c). */
#include <stdio.h>
#include <string.h>
#include "host_mi_status_types.h"
#include "host_vector_json.h"

static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

struct vector {
	char name[48];
	int bmp;
	int f0, f1, f2, f3;
	int a0, a1;
	int s0;
	int mg0, ro0;
	int e_sta, e_ld, e_lg, e_ap, e_ld_ap, e_start_ap;
	int e_scan, e_scan_ent, e_wps, e_mgmt, e_roch;
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

static int run_vector(struct vector *v)
{
	struct mi_state m;

	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_dv.padapters[i] = &g_if[i];
	}
	setup_iface(0, v->f0, v->a0, v->s0, v->mg0, v->ro0);
	setup_iface(1, v->f1, v->a1, 0, 0, 0);
	setup_iface(2, v->f2, 0, 0, 0, 0);
	setup_iface(3, v->f3, 0, 0, 0, 0);
	rtw_mi_status_by_ifbmp(&g_dv, (u8)(v->bmp ? v->bmp : 0xFF), &m);
	return mstate_eq(&m, v) ? 0 : -1;
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"bmp", "f0", "f1", "f2", "f3", "a0", "a1", "s0", "mg0", "ro0",
		"e_sta", "e_ld", "e_lg", "e_ap", "e_ld_ap", "e_start_ap",
		"e_scan", "e_scan_ent", "e_wps", "e_mgmt", "e_roch", NULL
	};
	int *vals[] = {
		&v->bmp, &v->f0, &v->f1, &v->f2, &v->f3, &v->a0, &v->a1, &v->s0,
		&v->mg0, &v->ro0, &v->e_sta, &v->e_ld, &v->e_lg, &v->e_ap,
		&v->e_ld_ap, &v->e_start_ap, &v->e_scan, &v->e_scan_ent,
		&v->e_wps, &v->e_mgmt, &v->e_roch
	};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (int i = 0; keys[i]; i++)
		host_json_parse_int_in(obj, len, keys[i], vals[i]);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int fail = 0;
	const char *path = (argc > 1) ? argv[1] : "mi_status_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 16, parse_vector_object, &n))
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
