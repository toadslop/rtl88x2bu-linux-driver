// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-183 PS_RDY_CHECK. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_ps_rdy.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int time;
	int lps_deny_time;
	int fw_state;
	int b_in_suspend;
	int wowlan_mode;
	int wowlan_ap_mode;
	int set_scan_deny;
	int roch;
	int tdls_linked;
	int set_radar_detect;
	int dot11_auth;
	int binstall_grpkey;
	int pwr_mgmt;
	int expect;
};

static _adapter_ps_rdy g_a;
static struct rf_ctl_t g_rfctl;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->expect = 1;
	v->pwr_mgmt = 1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "time", &v->time);
	host_json_parse_int_in(obj, len, "lps_deny_time", &v->lps_deny_time);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "b_in_suspend", &v->b_in_suspend);
	host_json_parse_int_in(obj, len, "wowlan_mode", &v->wowlan_mode);
	host_json_parse_int_in(obj, len, "wowlan_ap_mode", &v->wowlan_ap_mode);
	host_json_parse_int_in(obj, len, "set_scan_deny", &v->set_scan_deny);
	host_json_parse_int_in(obj, len, "roch", &v->roch);
	host_json_parse_int_in(obj, len, "tdls_linked", &v->tdls_linked);
	host_json_parse_int_in(obj, len, "set_radar_detect", &v->set_radar_detect);
	host_json_parse_int_in(obj, len, "dot11_auth", &v->dot11_auth);
	host_json_parse_int_in(obj, len, "binstall_grpkey", &v->binstall_grpkey);
	host_json_parse_int_in(obj, len, "pwr_mgmt", &v->pwr_mgmt);
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv_ps_rdy *p;
	u8 got;

	host_pwrctrl_ps_rdy_set_time((systime)v->time);
	host_pwrctrl_ps_rdy_set_scan_deny((u8)v->set_scan_deny);
	host_pwrctrl_ps_rdy_set_roch((u8)v->roch);
	host_pwrctrl_ps_rdy_set_pwr_mgmt((u8)v->pwr_mgmt);

	memset(&g_a, 0, sizeof(g_a));
	memset(&g_rfctl, 0, sizeof(g_rfctl));
	g_a.rfctl = &g_rfctl;

	p = adapter_to_pwrctl_ps_rdy(&g_a);
	p->lps_deny_time = (systime)v->lps_deny_time;
	p->bInSuspend = (u8)v->b_in_suspend;
	p->wowlan_mode = (u8)v->wowlan_mode;
	p->wowlan_ap_mode = (u8)v->wowlan_ap_mode;
	g_a.mlmepriv.fw_state = (u32)v->fw_state;
	g_a.tdlsinfo.link_established = (u8)v->tdls_linked;
	g_rfctl.radar_detect_enabled = (u8)v->set_radar_detect;
	g_a.securitypriv.dot11AuthAlgrthm = (u32)v->dot11_auth;
	g_a.securitypriv.binstallGrpkey = (u8)v->binstall_grpkey;

	got = PS_RDY_CHECK(&g_a);
	if ((int)got == v->expect) {
		printf("PASS %s\n", v->name);
		return 0;
	}
	fprintf(stderr, "FAIL %s got=%u expect=%d\n", v->name, got, v->expect);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ps_rdy_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
