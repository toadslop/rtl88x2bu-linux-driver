// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_set_ps.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int ps_mode, smart_ps, bcn_ant_mode;
	int initial_pwr_mode, initial_smart_ps, initial_bcn_ant_mode;
	int has_initial_smart_ps, has_initial_bcn_ant_mode;
	int lps_level, wowlan_mode, sys_clkr_in_32k, alives, ps_rdy, fw_asoc;
	int expect_lps_leave_cnts, expect_lps_enter_cnts, expect_exec_lps, expect_rsvd_page,
	    expect_default_port;
	int expect_hwreg_count, expect_hwreg0_var, expect_hwreg0_val;
	int expect_hwreg1_var, expect_hwreg1_val, expect_wow_fail;
	int expect_pwr_mode, expect_rpwm, expect_exec_lps_mode, expect_b_fw_current_in_ps;
	int expect_smart_ps, expect_bcn_ant_mode, has_expect_smart_ps, has_expect_bcn_ant_mode;
	u8 has_pwr, has_rpwm, has_exec_mode, has_b_fw;
};

static _adapter g_adapter;

static int parse_vec(const char *o, size_t l, void *v)
{
	struct vector *vv = v;
	memset(vv, 0, sizeof(*vv));
	vv->ps_rdy = 1;
	vv->fw_asoc = 1;
	if (host_json_parse_string_in(o, l, "name", vv->name, sizeof(vv->name)))
		return -1;
	host_json_parse_int_in(o, l, "ps_mode", &vv->ps_mode);
	host_json_parse_int_in(o, l, "smart_ps", &vv->smart_ps);
	host_json_parse_int_in(o, l, "bcn_ant_mode", &vv->bcn_ant_mode);
	host_json_parse_int_in(o, l, "initial_pwr_mode", &vv->initial_pwr_mode);
	if (host_json_find_key_in(o, l, "initial_smart_ps")) {
		host_json_parse_int_in(o, l, "initial_smart_ps", &vv->initial_smart_ps);
		vv->has_initial_smart_ps = 1;
	}
	if (host_json_find_key_in(o, l, "initial_bcn_ant_mode")) {
		host_json_parse_int_in(o, l, "initial_bcn_ant_mode",
				       &vv->initial_bcn_ant_mode);
		vv->has_initial_bcn_ant_mode = 1;
	}
	host_json_parse_int_in(o, l, "lps_level", &vv->lps_level);
	host_json_parse_int_in(o, l, "wowlan_mode", &vv->wowlan_mode);
	host_json_parse_int_in(o, l, "sys_clkr_in_32k", &vv->sys_clkr_in_32k);
	host_json_parse_int_in(o, l, "alives", &vv->alives);
	host_json_parse_int_in(o, l, "ps_rdy", &vv->ps_rdy);
	host_json_parse_int_in(o, l, "fw_asoc", &vv->fw_asoc);
	host_json_parse_int_in(o, l, "expect_lps_leave_cnts", &vv->expect_lps_leave_cnts);
	host_json_parse_int_in(o, l, "expect_lps_enter_cnts", &vv->expect_lps_enter_cnts);
	host_json_parse_int_in(o, l, "expect_exec_lps", &vv->expect_exec_lps);
	host_json_parse_int_in(o, l, "expect_rsvd_page", &vv->expect_rsvd_page);
	host_json_parse_int_in(o, l, "expect_default_port", &vv->expect_default_port);
	host_json_parse_int_in(o, l, "expect_hwreg_count", &vv->expect_hwreg_count);
	host_json_parse_int_in(o, l, "expect_hwreg0_var", &vv->expect_hwreg0_var);
	host_json_parse_int_in(o, l, "expect_hwreg0_val", &vv->expect_hwreg0_val);
	host_json_parse_int_in(o, l, "expect_hwreg1_var", &vv->expect_hwreg1_var);
	host_json_parse_int_in(o, l, "expect_hwreg1_val", &vv->expect_hwreg1_val);
	host_json_parse_int_in(o, l, "expect_wow_fail", &vv->expect_wow_fail);
	if (!host_json_parse_int_in(o, l, "expect_pwr_mode", &vv->expect_pwr_mode))
		vv->has_pwr = 1;
	if (!host_json_parse_int_in(o, l, "expect_rpwm", &vv->expect_rpwm))
		vv->has_rpwm = 1;
	if (!host_json_parse_int_in(o, l, "expect_exec_lps_mode", &vv->expect_exec_lps_mode))
		vv->has_exec_mode = 1;
	if (!host_json_parse_int_in(o, l, "expect_b_fw_current_in_ps", &vv->expect_b_fw_current_in_ps))
		vv->has_b_fw = 1;
	if (!host_json_parse_int_in(o, l, "expect_smart_ps", &vv->expect_smart_ps))
		vv->has_expect_smart_ps = 1;
	if (!host_json_parse_int_in(o, l, "expect_bcn_ant_mode", &vv->expect_bcn_ant_mode))
		vv->has_expect_bcn_ant_mode = 1;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p = adapter_to_pwrctl(&g_adapter);
	struct host_pwrctrl_set_ps_trace *t;
	memset(&g_adapter, 0, sizeof(g_adapter));
	host_pwrctrl_set_ps_reset_trace();
	host_pwrctrl_set_ps_reset_hal();
	host_pwrctrl_set_ps_set_sys_clkr_in_32k((u8)v->sys_clkr_in_32k);
	host_pwrctrl_set_ps_set_ps_rdy((u8)v->ps_rdy);
	host_pwrctrl_set_ps_set_fw_asoc((u8)v->fw_asoc);
	g_adapter.mlmepriv.fw_state = v->fw_asoc ? WIFI_ASOC_STATE : 0;
	p->pwr_mode = (u8)v->initial_pwr_mode;
	p->smart_ps = (u8)(v->has_initial_smart_ps ? v->initial_smart_ps : v->smart_ps);
	p->bcn_ant_mode =
		(u8)(v->has_initial_bcn_ant_mode ? v->initial_bcn_ant_mode :
						  v->bcn_ant_mode);
	p->lps_level = (u8)v->lps_level;
	p->wowlan_mode = (u8)v->wowlan_mode;
	p->alives = (u8)v->alives;
	p->bFwCurrentInPSMode = 1;
	rtw_set_ps_mode(&g_adapter, (u8)v->ps_mode, (u8)v->smart_ps, (u8)v->bcn_ant_mode, "H");
	t = host_pwrctrl_set_ps_get_trace();
	if ((int)t->lps_leave_cnts != v->expect_lps_leave_cnts ||
	    (int)t->lps_enter_cnts != v->expect_lps_enter_cnts ||
	    (int)t->exec_lps_called != v->expect_exec_lps ||
	    (int)t->rsvd_page_calls != v->expect_rsvd_page ||
	    (int)t->default_port_calls != v->expect_default_port ||
	    (int)t->hwreg_count != v->expect_hwreg_count ||
	    (int)t->wow_leave_fail_cnt != v->expect_wow_fail)
		return fprintf(stderr, "FAIL %s trace\n", v->name), 1;
	if (v->has_pwr && (int)t->pwr_mode != v->expect_pwr_mode)
		return fprintf(stderr, "FAIL %s pwr\n", v->name), 1;
	if (v->has_rpwm && (int)t->rpwm_val != v->expect_rpwm)
		return fprintf(stderr, "FAIL %s rpwm\n", v->name), 1;
	if (v->has_exec_mode && (int)t->exec_lps_ps_mode != v->expect_exec_lps_mode)
		return fprintf(stderr, "FAIL %s exec mode\n", v->name), 1;
	if (v->has_b_fw && (int)t->b_fw_current_in_ps != v->expect_b_fw_current_in_ps)
		return fprintf(stderr, "FAIL %s bfw\n", v->name), 1;
	if (v->has_expect_smart_ps && (int)t->smart_ps != v->expect_smart_ps)
		return fprintf(stderr, "FAIL %s smart_ps\n", v->name), 1;
	if (v->has_expect_bcn_ant_mode && (int)t->bcn_ant_mode != v->expect_bcn_ant_mode)
		return fprintf(stderr, "FAIL %s bcn_ant\n", v->name), 1;
	if (v->expect_hwreg_count &&
	    ((int)t->hwreg_var[0] != v->expect_hwreg0_var ||
	     (int)t->hwreg_val[0] != v->expect_hwreg0_val))
		return fprintf(stderr, "FAIL %s hw0\n", v->name), 1;
	if (v->expect_hwreg_count > 1 &&
	    ((int)t->hwreg_var[1] != v->expect_hwreg1_var ||
	     (int)t->hwreg_val[1] != v->expect_hwreg1_val))
		return fprintf(stderr, "FAIL %s hw1\n", v->name), 1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "set_ps_vectors.json";
	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 16, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
