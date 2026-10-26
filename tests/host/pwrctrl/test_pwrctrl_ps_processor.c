// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-173 rtw_ps_processor. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_ps_processor.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	int ps_deny;
	int b_in_suspend;
	int ips_mode_req;
	int rf_pwrstate;
	int pwr_state_check_cnts;
	int unassoc_idle;
	int expect_ips_enter;
	int expect_change_rfpwrstate;
	int expect_ps_processing_after;
	int has_expect_change_rfpwrstate;
	int has_expect_ps_processing_after;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "ps_deny", &v->ps_deny);
	host_json_parse_int_in(obj, len, "b_in_suspend", &v->b_in_suspend);
	host_json_parse_int_in(obj, len, "ips_mode_req", &v->ips_mode_req);
	host_json_parse_int_in(obj, len, "rf_pwrstate", &v->rf_pwrstate);
	host_json_parse_int_in(obj, len, "pwr_state_check_cnts", &v->pwr_state_check_cnts);
	if (host_json_parse_int_in(obj, len, "unassoc_idle", &v->unassoc_idle) != 0)
		v->unassoc_idle = 1;
	host_json_parse_int_in(obj, len, "expect_ips_enter", &v->expect_ips_enter);
	if (host_json_parse_int_in(obj, len, "expect_change_rfpwrstate",
				   &v->expect_change_rfpwrstate) == 0)
		v->has_expect_change_rfpwrstate = 1;
	if (host_json_parse_int_in(obj, len, "expect_ps_processing_after",
				   &v->expect_ps_processing_after) == 0)
		v->has_expect_ps_processing_after = 1;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p;
	struct host_pwrctrl_ps_processor_trace *tr;

	memset(&g_adapter, 0, sizeof(g_adapter));
	host_pwrctrl_ps_proc_reset_trace();
	host_pwrctrl_ps_proc_set_unassoc_idle((u8)v->unassoc_idle);

	p = adapter_to_pwrctl(&g_adapter);
	p->ps_deny = (u32)v->ps_deny;
	p->bInSuspend = (u8)v->b_in_suspend;
	p->ips_mode_req = (u8)v->ips_mode_req;
	p->rf_pwrstate = (u8)v->rf_pwrstate;
	p->pwr_state_check_cnts = (u32)v->pwr_state_check_cnts;

	rtw_ps_processor(&g_adapter);

	tr = host_pwrctrl_ps_proc_get_trace();
	if ((int)tr->ips_enter != v->expect_ips_enter) {
		fprintf(stderr, "FAIL %s ips_enter=%u expect %d\n", v->name,
			tr->ips_enter, v->expect_ips_enter);
		return 1;
	}
	if (v->has_expect_change_rfpwrstate &&
	    (int)p->change_rfpwrstate != v->expect_change_rfpwrstate) {
		fprintf(stderr, "FAIL %s change_rfpwrstate=%u expect %d\n", v->name,
			p->change_rfpwrstate, v->expect_change_rfpwrstate);
		return 1;
	}
	if (v->has_expect_ps_processing_after &&
	    (int)p->ps_processing != v->expect_ps_processing_after) {
		fprintf(stderr, "FAIL %s ps_processing=%u expect %d\n", v->name,
			p->ps_processing, v->expect_ps_processing_after);
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
	const char *path = argc > 1 ? argv[1] : "ps_processor_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
