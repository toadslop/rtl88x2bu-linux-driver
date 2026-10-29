// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-189 rtw_init_pwrctrl_priv part 1. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_init.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int ips_mode;
	int lps_level;
	int mp_mode;
	int power_mgnt;
	int smart_ps;
	int adapter_type;
	int has_expect_rf_pwrstate;
	int expect_rf_pwrstate;
	int has_expect_ips_mode;
	int expect_ips_mode;
	int has_expect_power_mgnt;
	int expect_power_mgnt;
	int has_expect_b_leisure_ps;
	int expect_b_leisure_ps;
	int has_expect_cpwm;
	int expect_cpwm;
	int has_expect_tog;
	int expect_tog;
	int has_expect_wowlan_mode;
	int expect_wowlan_mode;
	int has_expect_set_rpwm_hwreg;
	int expect_set_rpwm_hwreg;
	int has_expect_init_workitem;
	int expect_init_workitem;
	int has_expect_init_timer;
	int expect_init_timer;
	int has_expect_create_wq;
	int expect_create_wq;
	int has_expect_skipped;
	int expect_skipped;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "ips_mode", &v->ips_mode);
	host_json_parse_int_in(obj, len, "lps_level", &v->lps_level);
	host_json_parse_int_in(obj, len, "mp_mode", &v->mp_mode);
	host_json_parse_int_in(obj, len, "power_mgnt", &v->power_mgnt);
	host_json_parse_int_in(obj, len, "smart_ps", &v->smart_ps);
	host_json_parse_int_in(obj, len, "adapter_type", &v->adapter_type);
	if (host_json_parse_int_in(obj, len, "expect_rf_pwrstate",
				   &v->expect_rf_pwrstate) == 0)
		v->has_expect_rf_pwrstate = 1;
	if (host_json_parse_int_in(obj, len, "expect_ips_mode", &v->expect_ips_mode) == 0)
		v->has_expect_ips_mode = 1;
	if (host_json_parse_int_in(obj, len, "expect_power_mgnt", &v->expect_power_mgnt) == 0)
		v->has_expect_power_mgnt = 1;
	if (host_json_parse_int_in(obj, len, "expect_b_leisure_ps",
				   &v->expect_b_leisure_ps) == 0)
		v->has_expect_b_leisure_ps = 1;
	if (host_json_parse_int_in(obj, len, "expect_cpwm", &v->expect_cpwm) == 0)
		v->has_expect_cpwm = 1;
	if (host_json_parse_int_in(obj, len, "expect_tog", &v->expect_tog) == 0)
		v->has_expect_tog = 1;
	if (host_json_parse_int_in(obj, len, "expect_wowlan_mode", &v->expect_wowlan_mode) == 0)
		v->has_expect_wowlan_mode = 1;
	if (host_json_parse_int_in(obj, len, "expect_set_rpwm_hwreg",
				   &v->expect_set_rpwm_hwreg) == 0)
		v->has_expect_set_rpwm_hwreg = 1;
	if (host_json_parse_int_in(obj, len, "expect_init_workitem",
				   &v->expect_init_workitem) == 0)
		v->has_expect_init_workitem = 1;
	if (host_json_parse_int_in(obj, len, "expect_init_timer",
				   &v->expect_init_timer) == 0)
		v->has_expect_init_timer = 1;
	if (host_json_parse_int_in(obj, len, "expect_create_wq",
				   &v->expect_create_wq) == 0)
		v->has_expect_create_wq = 1;
	if (host_json_parse_int_in(obj, len, "expect_skipped", &v->expect_skipped) == 0)
		v->has_expect_skipped = 1;
	return 0;
}

static int run_vec(struct vector *v)
{
	struct pwrctrl_priv_init *p;
	struct host_pwrctrl_init_trace *tr;

	host_pwrctrl_init_reset_trace();
	host_pwrctrl_init_set_current_time(100);
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.adapter_type = (u8)v->adapter_type;
	g_adapter.registrypriv.ips_mode = (u8)v->ips_mode;
	g_adapter.registrypriv.lps_level = (u8)v->lps_level;
	g_adapter.registrypriv.mp_mode = (u8)v->mp_mode;
	g_adapter.registrypriv.power_mgnt = (u8)v->power_mgnt;
	g_adapter.registrypriv.smart_ps = (u8)v->smart_ps;

	rtw_init_pwrctrl_priv_part1(&g_adapter);
	p = adapter_to_pwrctl(&g_adapter);
	tr = host_pwrctrl_init_get_trace();

	if (v->has_expect_skipped &&
	    (int)tr->skipped_non_primary != v->expect_skipped) {
		fprintf(stderr, "FAIL %s skipped\n", v->name);
		return 1;
	}
	if (v->has_expect_rf_pwrstate &&
	    (int)p->rf_pwrstate != v->expect_rf_pwrstate) {
		fprintf(stderr, "FAIL %s rf_pwrstate\n", v->name);
		return 1;
	}
	if (v->has_expect_ips_mode && (int)p->ips_mode != v->expect_ips_mode) {
		fprintf(stderr, "FAIL %s ips_mode\n", v->name);
		return 1;
	}
	if (v->has_expect_power_mgnt && (int)p->power_mgnt != v->expect_power_mgnt) {
		fprintf(stderr, "FAIL %s power_mgnt\n", v->name);
		return 1;
	}
	if (v->has_expect_b_leisure_ps &&
	    (int)p->bLeisurePs != v->expect_b_leisure_ps) {
		fprintf(stderr, "FAIL %s bLeisurePs\n", v->name);
		return 1;
	}
	if (v->has_expect_cpwm && (int)p->cpwm != v->expect_cpwm) {
		fprintf(stderr, "FAIL %s cpwm\n", v->name);
		return 1;
	}
	if (v->has_expect_tog && (int)p->tog != v->expect_tog) {
		fprintf(stderr, "FAIL %s tog\n", v->name);
		return 1;
	}
	if (v->has_expect_wowlan_mode &&
	    (int)p->wowlan_mode != v->expect_wowlan_mode) {
		fprintf(stderr, "FAIL %s wowlan_mode\n", v->name);
		return 1;
	}
	if (v->has_expect_set_rpwm_hwreg &&
	    (int)tr->set_rpwm_hwreg_calls != v->expect_set_rpwm_hwreg) {
		fprintf(stderr, "FAIL %s hwreg\n", v->name);
		return 1;
	}
	if (v->has_expect_init_workitem &&
	    (int)tr->init_workitem_calls != v->expect_init_workitem) {
		fprintf(stderr, "FAIL %s workitem got %u expect %d\n", v->name,
			tr->init_workitem_calls, v->expect_init_workitem);
		return 1;
	}
	if (v->has_expect_init_timer &&
	    (int)tr->init_timer_calls != v->expect_init_timer) {
		fprintf(stderr, "FAIL %s init_timer got %u expect %d\n", v->name,
			tr->init_timer_calls, v->expect_init_timer);
		return 1;
	}
	if (v->has_expect_create_wq &&
	    (int)tr->create_wq_calls != v->expect_create_wq) {
		fprintf(stderr, "FAIL %s create_wq got %u expect %d\n", v->name,
			tr->create_wq_calls, v->expect_create_wq);
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
	const char *path = argc > 1 ? argv[1] : "init_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("ALL PASS (%zu vectors)\n", n);
	return bad ? 1 : 0;
}
