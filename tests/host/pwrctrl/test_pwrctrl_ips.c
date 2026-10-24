// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-167 IPS enter/leave. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_ips.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char action[16];
	int adapter_type;
	int change_rfpwrstate;
	int ips_mode_req;
	int rf_pwrstate;
	int expect_enter_cnts;
	int expect_leave_cnts;
	int expect_rf_pwrstate;
	int expect_rf_pwrstate_after;
	int expect_bpower_saving;
	int expect_ips_pwr_down;
	int expect_ips_pwr_up;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "action", v->action, sizeof(v->action));
	host_json_parse_int_in(obj, len, "adapter_type", &v->adapter_type);
	host_json_parse_int_in(obj, len, "change_rfpwrstate", &v->change_rfpwrstate);
	host_json_parse_int_in(obj, len, "ips_mode_req", &v->ips_mode_req);
	host_json_parse_int_in(obj, len, "rf_pwrstate", &v->rf_pwrstate);
	host_json_parse_int_in(obj, len, "expect_enter_cnts", &v->expect_enter_cnts);
	host_json_parse_int_in(obj, len, "expect_leave_cnts", &v->expect_leave_cnts);
	host_json_parse_int_in(obj, len, "expect_rf_pwrstate", &v->expect_rf_pwrstate);
	host_json_parse_int_in(obj, len, "expect_rf_pwrstate_after", &v->expect_rf_pwrstate_after);
	host_json_parse_int_in(obj, len, "expect_bpower_saving", &v->expect_bpower_saving);
	host_json_parse_int_in(obj, len, "expect_ips_pwr_down", &v->expect_ips_pwr_down);
	host_json_parse_int_in(obj, len, "expect_ips_pwr_up", &v->expect_ips_pwr_up);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv *p;
	struct host_pwrctrl_ips_trace *tr;

	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.adapter_type = (u8)(v->adapter_type >= 0 ? v->adapter_type : 0);
	p = adapter_to_pwrctl(&g_adapter);
	p->change_rfpwrstate = (u8)v->change_rfpwrstate;
	p->ips_mode_req = (u8)v->ips_mode_req;
	p->rf_pwrstate = (u8)v->rf_pwrstate;

	host_pwrctrl_ips_reset_trace();
	host_pwrctrl_ips_set_pwr_up_result(_SUCCESS);

	if (strcmp(v->action, "enter") == 0) {
		ips_enter(&g_adapter);
		if ((int)p->ips_enter_cnts != v->expect_enter_cnts ||
		    (int)p->rf_pwrstate != v->expect_rf_pwrstate ||
		    (int)p->bpower_saving != v->expect_bpower_saving) {
			fprintf(stderr, "FAIL %s enter state enter=%u rf=%u ps=%u\n",
				v->name, p->ips_enter_cnts, p->rf_pwrstate, p->bpower_saving);
			return 1;
		}
		tr = host_pwrctrl_ips_get_trace();
		if ((int)tr->ips_pwr_down != v->expect_ips_pwr_down) {
			fprintf(stderr, "FAIL %s enter trace pwr_down=%u expect %d\n",
				v->name, tr->ips_pwr_down, v->expect_ips_pwr_down);
			return 1;
		}
	}

	if (strcmp(v->action, "leave") == 0) {
		ips_leave(&g_adapter);
		if ((int)p->ips_leave_cnts != v->expect_leave_cnts) {
			fprintf(stderr, "FAIL %s leave cnt=%u expect %d\n",
				v->name, p->ips_leave_cnts, v->expect_leave_cnts);
			return 1;
		}
		if (v->expect_rf_pwrstate_after >= 0 &&
		    (int)p->rf_pwrstate != v->expect_rf_pwrstate_after) {
			fprintf(stderr, "FAIL %s leave rf=%u expect %d\n",
				v->name, p->rf_pwrstate, v->expect_rf_pwrstate_after);
			return 1;
		}
		tr = host_pwrctrl_ips_get_trace();
		if ((int)tr->ips_pwr_up != v->expect_ips_pwr_up) {
			fprintf(stderr, "FAIL %s leave trace pwr_up=%u expect %d\n",
				v->name, tr->ips_pwr_up, v->expect_ips_pwr_up);
			return 1;
		}
	}

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "ips_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
