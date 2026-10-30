// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-184 PM ioctl setters. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_pm.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_FN 16

struct vector {
	char name[MAX_NAME];
	char fn[MAX_FN];
	int mode;
	int initial_power_mgnt;
	int initial_lps_level;
	int surprise_removed;
	int pwr_wakeup_fail;
	int expect_ret;
	int expect_power_mgnt;
	int expect_bLeisurePs;
	int expect_LpsIdleCount;
	int expect_lps_level;
	int expect_ips_mode_req;
	int expect_ips_mode_req_calls;
	int expect_leave_all_ps;
	int expect_pwr_wakeup_calls;
	int has_expect_power_mgnt;
	int has_expect_bLeisurePs;
	int has_expect_LpsIdleCount;
	int has_expect_lps_level;
	int has_expect_ips_mode_req;
};

static _adapter_pm g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "mode", &v->mode);
	host_json_parse_int_in(obj, len, "initial_power_mgnt", &v->initial_power_mgnt);
	host_json_parse_int_in(obj, len, "initial_lps_level", &v->initial_lps_level);
	host_json_parse_int_in(obj, len, "surprise_removed", &v->surprise_removed);
	host_json_parse_int_in(obj, len, "pwr_wakeup_fail", &v->pwr_wakeup_fail);
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	if (!host_json_parse_int_in(obj, len, "expect_power_mgnt", &v->expect_power_mgnt))
		v->has_expect_power_mgnt = 1;
	if (!host_json_parse_int_in(obj, len, "expect_bLeisurePs", &v->expect_bLeisurePs))
		v->has_expect_bLeisurePs = 1;
	if (!host_json_parse_int_in(obj, len, "expect_LpsIdleCount", &v->expect_LpsIdleCount))
		v->has_expect_LpsIdleCount = 1;
	if (!host_json_parse_int_in(obj, len, "expect_lps_level", &v->expect_lps_level))
		v->has_expect_lps_level = 1;
	if (!host_json_parse_int_in(obj, len, "expect_ips_mode_req", &v->expect_ips_mode_req))
		v->has_expect_ips_mode_req = 1;
	host_json_parse_int_in(obj, len, "expect_ips_mode_req_calls",
			       &v->expect_ips_mode_req_calls);
	host_json_parse_int_in(obj, len, "expect_leave_all_ps", &v->expect_leave_all_ps);
	host_json_parse_int_in(obj, len, "expect_pwr_wakeup_calls",
			       &v->expect_pwr_wakeup_calls);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv_pm *p;
	struct host_pwrctrl_pm_trace *tr;
	int ret;

	memset(&g_adapter, 0, sizeof(g_adapter));
	p = adapter_to_pwrctl_pm(&g_adapter);
	p->power_mgnt = (u8)v->initial_power_mgnt;
	p->lps_level = (u8)v->initial_lps_level;
	g_adapter.bSurpriseRemoved = (u8)v->surprise_removed;

	host_pwrctrl_pm_reset_trace();
	host_pwrctrl_pm_set_pwr_wakeup_result(v->pwr_wakeup_fail ? _FAIL : _SUCCESS);

	if (strcmp(v->fn, "lps") == 0)
		ret = rtw_pm_set_lps(&g_adapter, (u8)v->mode);
	else if (strcmp(v->fn, "lps_level") == 0)
		ret = rtw_pm_set_lps_level(&g_adapter, (u8)v->mode);
	else if (strcmp(v->fn, "ips") == 0)
		ret = rtw_pm_set_ips(&g_adapter, (u8)v->mode);
	else {
		fprintf(stderr, "FAIL %s unknown fn %s\n", v->name, v->fn);
		return 1;
	}

	if (ret != v->expect_ret) {
		fprintf(stderr, "FAIL %s ret=%d expect %d\n", v->name, ret, v->expect_ret);
		return 1;
	}

	tr = host_pwrctrl_pm_get_trace();
	if (tr->leave_all_ps != v->expect_leave_all_ps) {
		fprintf(stderr, "FAIL %s leave_all_ps=%d expect %d\n", v->name,
			tr->leave_all_ps, v->expect_leave_all_ps);
		return 1;
	}
	if (tr->ips_mode_req_calls != v->expect_ips_mode_req_calls) {
		fprintf(stderr, "FAIL %s ips_req_calls=%d expect %d\n", v->name,
			tr->ips_mode_req_calls, v->expect_ips_mode_req_calls);
		return 1;
	}
	if (tr->pwr_wakeup_calls != v->expect_pwr_wakeup_calls) {
		fprintf(stderr, "FAIL %s pwr_wakeup=%d expect %d\n", v->name,
			tr->pwr_wakeup_calls, v->expect_pwr_wakeup_calls);
		return 1;
	}
	if (v->has_expect_power_mgnt && (int)p->power_mgnt != v->expect_power_mgnt) {
		fprintf(stderr, "FAIL %s power_mgnt=%u expect %d\n", v->name, p->power_mgnt,
			v->expect_power_mgnt);
		return 1;
	}
	if (v->has_expect_bLeisurePs && (int)p->bLeisurePs != v->expect_bLeisurePs) {
		fprintf(stderr, "FAIL %s bLeisurePs=%u expect %d\n", v->name, p->bLeisurePs,
			v->expect_bLeisurePs);
		return 1;
	}
	if (v->has_expect_LpsIdleCount && (int)p->LpsIdleCount != v->expect_LpsIdleCount) {
		fprintf(stderr, "FAIL %s LpsIdleCount=%u expect %d\n", v->name, p->LpsIdleCount,
			v->expect_LpsIdleCount);
		return 1;
	}
	if (v->has_expect_lps_level && (int)p->lps_level != v->expect_lps_level) {
		fprintf(stderr, "FAIL %s lps_level=%u expect %d\n", v->name, p->lps_level,
			v->expect_lps_level);
		return 1;
	}
	if (v->has_expect_ips_mode_req && (int)p->ips_mode_req != v->expect_ips_mode_req) {
		fprintf(stderr, "FAIL %s ips_mode_req=%u expect %d\n", v->name, p->ips_mode_req,
			v->expect_ips_mode_req);
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
	const char *path = argc > 1 ? argv[1] : "pm_setters_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
