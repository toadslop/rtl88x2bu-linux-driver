// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-181 rtw_exec_lps / rtw_lps_rfon_ctrl. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_lps_exec.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char fn[MAX_NAME];
	int ps_mode;
	int fw_state;
	int b_fw_current_in_ps;
	int pwr_mode;
	int rfon_ctrl;
	int expect_hwreg_count;
	int expect_hwreg0_var;
	int expect_hwreg0_val;
	int expect_hwreg1_var;
	int expect_hwreg1_val;
	int has_expect_hwreg1;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn));
	host_json_parse_int_in(obj, len, "ps_mode", &v->ps_mode);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "b_fw_current_in_ps", &v->b_fw_current_in_ps);
	host_json_parse_int_in(obj, len, "pwr_mode", &v->pwr_mode);
	host_json_parse_int_in(obj, len, "rfon_ctrl", &v->rfon_ctrl);
	host_json_parse_int_in(obj, len, "expect_hwreg_count", &v->expect_hwreg_count);
	host_json_parse_int_in(obj, len, "expect_hwreg0_var", &v->expect_hwreg0_var);
	host_json_parse_int_in(obj, len, "expect_hwreg0_val", &v->expect_hwreg0_val);
	if (host_json_parse_int_in(obj, len, "expect_hwreg1_var", &v->expect_hwreg1_var) == 0) {
		v->has_expect_hwreg1 = 1;
		host_json_parse_int_in(obj, len, "expect_hwreg1_val", &v->expect_hwreg1_val);
	}
	return 0;
}

static int check_trace(struct vector *v, struct host_pwrctrl_lps_exec_trace *tr)
{
	if ((int)tr->hwreg_count != v->expect_hwreg_count) {
		fprintf(stderr, "FAIL %s hwreg_count=%u expect %d\n", v->name,
			tr->hwreg_count, v->expect_hwreg_count);
		return 1;
	}
	if (v->expect_hwreg_count > 0) {
		if ((int)tr->hwreg[0].var != v->expect_hwreg0_var ||
		    (int)tr->hwreg[0].val != v->expect_hwreg0_val) {
			fprintf(stderr,
				"FAIL %s hwreg[0] var=%u val=%u expect var=%d val=%d\n",
				v->name, tr->hwreg[0].var, tr->hwreg[0].val,
				v->expect_hwreg0_var, v->expect_hwreg0_val);
			return 1;
		}
	}
	if (v->has_expect_hwreg1 && v->expect_hwreg_count > 1) {
		if ((int)tr->hwreg[1].var != v->expect_hwreg1_var ||
		    (int)tr->hwreg[1].val != v->expect_hwreg1_val) {
			fprintf(stderr,
				"FAIL %s hwreg[1] var=%u val=%u expect var=%d val=%d\n",
				v->name, tr->hwreg[1].var, tr->hwreg[1].val,
				v->expect_hwreg1_var, v->expect_hwreg1_val);
			return 1;
		}
	}
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p;
	struct host_pwrctrl_lps_exec_trace *tr;

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_pwrctrl_lps_exec_reset_trace();

	p = adapter_to_pwrctl(&g_adapter);
	p->bFwCurrentInPSMode = (u8)v->b_fw_current_in_ps;
	p->pwr_mode = (u8)v->pwr_mode;
	g_adapter.mlmepriv.fw_state = (u32)v->fw_state;

	if (v->fn[0] && !strcmp(v->fn, "rfon"))
		rtw_lps_rfon_ctrl(&g_adapter, (u8)v->rfon_ctrl);
	else
		rtw_exec_lps(&g_adapter, (u8)v->ps_mode);

	tr = host_pwrctrl_lps_exec_get_trace();
	return check_trace(v, tr);
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "lps_exec_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
