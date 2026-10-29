// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_pwrctrl_set_ps.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int ps_mode, smart_ps, bcn_ant_mode;
	int initial_pwr_mode, initial_smart_ps, initial_bcn_ant_mode;
	int lps_level;
	int expect_lps_leave_cnts, expect_rsvd_page, expect_default_port;
	int expect_hwreg_count, expect_hwreg0_var, expect_hwreg0_val;
};

static _adapter g_adapter;

static int parse_vec(const char *o, size_t l, void *v)
{
	struct vector *vv = v;

	memset(vv, 0, sizeof(*vv));
	if (host_json_parse_string_in(o, l, "name", vv->name, sizeof(vv->name)))
		return -1;
	host_json_parse_int_in(o, l, "ps_mode", &vv->ps_mode);
	host_json_parse_int_in(o, l, "smart_ps", &vv->smart_ps);
	host_json_parse_int_in(o, l, "bcn_ant_mode", &vv->bcn_ant_mode);
	host_json_parse_int_in(o, l, "initial_pwr_mode", &vv->initial_pwr_mode);
	host_json_parse_int_in(o, l, "initial_smart_ps", &vv->initial_smart_ps);
	host_json_parse_int_in(o, l, "initial_bcn_ant_mode", &vv->initial_bcn_ant_mode);
	host_json_parse_int_in(o, l, "lps_level", &vv->lps_level);
	host_json_parse_int_in(o, l, "expect_lps_leave_cnts", &vv->expect_lps_leave_cnts);
	host_json_parse_int_in(o, l, "expect_rsvd_page", &vv->expect_rsvd_page);
	host_json_parse_int_in(o, l, "expect_default_port", &vv->expect_default_port);
	host_json_parse_int_in(o, l, "expect_hwreg_count", &vv->expect_hwreg_count);
	host_json_parse_int_in(o, l, "expect_hwreg0_var", &vv->expect_hwreg0_var);
	host_json_parse_int_in(o, l, "expect_hwreg0_val", &vv->expect_hwreg0_val);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p = adapter_to_pwrctl(&g_adapter);
	struct host_pwrctrl_set_ps_trace *t;

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_pwrctrl_set_ps_reset_trace();
	p->pwr_mode = (u8)v->initial_pwr_mode;
	p->smart_ps = (u8)(v->initial_smart_ps ? v->initial_smart_ps : v->smart_ps);
	p->bcn_ant_mode = (u8)(v->initial_bcn_ant_mode ? v->initial_bcn_ant_mode :
						     v->bcn_ant_mode);
	p->lps_level = (u8)v->lps_level;
	rtw_set_ps_mode(&g_adapter, (u8)v->ps_mode, (u8)v->smart_ps, (u8)v->bcn_ant_mode, "H");
	t = host_pwrctrl_set_ps_get_trace();
	if ((int)p->lps_leave_cnts != v->expect_lps_leave_cnts ||
	    (int)t->rsvd_page_calls != v->expect_rsvd_page ||
	    (int)t->default_port_calls != v->expect_default_port ||
	    (int)t->hwreg_count != v->expect_hwreg_count) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return 1;
	}
	if (v->expect_hwreg_count &&
	    ((int)t->hwreg_var[0] != v->expect_hwreg0_var ||
	     (int)t->hwreg_val[0] != v->expect_hwreg0_val)) {
		fprintf(stderr, "FAIL %s hwreg\n", v->name);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "set_ps_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
