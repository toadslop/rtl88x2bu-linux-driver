// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-195 pwr wakeup + ips_deny. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_wakeup.h"
#include "host_vector_json.h"

#define MAX_VECTORS 12
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int time;
	int passing_ms;
	int fw_state;
	int bInSuspend;
	int do_late_resume;
	int rf_pwrstate;
	int bup;
	int ips_deffer_ms;
	int deny_ms;
	int ips_leave_fail;
	int expect_ret;
	int expect_leave_all;
	int expect_ips_leave;
	int expect_deny_time;
};

static _adapter_wakeup g_adapter;
static struct dvobj_priv_wakeup g_dvobj;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->bup = 1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "time", &v->time);
	host_json_parse_int_in(obj, len, "passing_ms", &v->passing_ms);
	host_json_parse_int_in(obj, len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, len, "bInSuspend", &v->bInSuspend);
	host_json_parse_int_in(obj, len, "do_late_resume", &v->do_late_resume);
	host_json_parse_int_in(obj, len, "rf_pwrstate", &v->rf_pwrstate);
	host_json_parse_int_in(obj, len, "bup", &v->bup);
	host_json_parse_int_in(obj, len, "ips_deffer_ms", &v->ips_deffer_ms);
	host_json_parse_int_in(obj, len, "deny_ms", &v->deny_ms);
	host_json_parse_int_in(obj, len, "ips_leave_fail", &v->ips_leave_fail);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_leave_all", &v->expect_leave_all);
	host_json_parse_int_in(obj, len, "expect_ips_leave", &v->expect_ips_leave);
	host_json_parse_int_in(obj, len, "expect_deny_time", &v->expect_deny_time);
	return 0;
}

static void setup_adapter(struct vector *v)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	g_dvobj.padapters[0] = &g_adapter;
	g_adapter.dvobj = &g_dvobj;
	g_adapter.mlmepriv.fw_state = (u32)v->fw_state;
	g_adapter.pwrctrlpriv.bInSuspend = (u8)v->bInSuspend;
	g_adapter.pwrctrlpriv.do_late_resume = (u8)v->do_late_resume;
	g_adapter.pwrctrlpriv.rf_pwrstate = (u8)v->rf_pwrstate;
	g_adapter.bup = (u8)v->bup;
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_wakeup_trace *tr;
	int ret;

	host_pwrctrl_wakeup_reset_trace();
	host_pwrctrl_wakeup_set_passing_ms((u32)v->passing_ms);
	if (v->ips_leave_fail)
		host_pwrctrl_wakeup_set_ips_leave_result(_FAIL);

	setup_adapter(v);
	host_pwrctrl_wakeup_set_time((u32)v->time);

	if (v->deny_ms > 0) {
		g_adapter.pwrctrlpriv.ips_deny_time = 0;
		host_pwrctrl_wakeup_set_passing_ms(0);
		host_pwrctrl_wakeup_set_time((u32)v->time);
		rtw_set_ips_deny(&g_adapter, (u32)v->deny_ms);
		if ((s32)g_adapter.pwrctrlpriv.ips_deny_time != v->expect_deny_time) {
			fprintf(stderr, "FAIL %s deny_time=%u expect=%d\n", v->name,
				g_adapter.pwrctrlpriv.ips_deny_time, v->expect_deny_time);
			return 1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	ret = _rtw_pwr_wakeup(&g_adapter, (u32)v->ips_deffer_ms, "test");
	tr = host_pwrctrl_wakeup_get_trace();
	if (ret != v->expect_ret || tr->leave_all_ps != (u32)v->expect_leave_all ||
	    tr->ips_leave_calls != (u32)v->expect_ips_leave) {
		fprintf(stderr,
			"FAIL %s ret=%d leave=%u ips=%u (expect ret=%d leave=%d ips=%d)\n",
			v->name, ret, tr->leave_all_ps, tr->ips_leave_calls, v->expect_ret,
			v->expect_leave_all, v->expect_ips_leave);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "wakeup_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
