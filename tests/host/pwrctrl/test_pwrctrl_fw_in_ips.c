// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-185 FW LPS in IPS. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_fw_ips.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
struct vector {
	char name[MAX_NAME];
	int netif_up_val;
	int enable;
	int ips_mode_req;
	int hmetfr0;
	int hmetfr1;
	int hmetfr2;
	int hmetfr_count;
	int reg_cr0;
	int reg_cr1;
	int reg_cr2;
	int reg_cr_count;
	int cpwm_orig;
	int cpwm_after;
	int cpwm_reads_before_toggle;
	int expect_h2c_count;
	int expect_h2c_parm0;
	int expect_btcoex_calls;
	int expect_btcoex_mode;
	int expect_min_read8;
	int expect_hwreg_count_min;
	int has_expect_h2c_parm0;
};

static _adapter_fw_ips g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "netif_up_val", &v->netif_up_val);
	host_json_parse_int_in(obj, len, "enable", &v->enable);
	host_json_parse_int_in(obj, len, "ips_mode_req", &v->ips_mode_req);
	host_json_parse_int_in(obj, len, "hmetfr0", &v->hmetfr0);
	host_json_parse_int_in(obj, len, "hmetfr1", &v->hmetfr1);
	host_json_parse_int_in(obj, len, "hmetfr2", &v->hmetfr2);
	host_json_parse_int_in(obj, len, "hmetfr_count", &v->hmetfr_count);
	host_json_parse_int_in(obj, len, "reg_cr0", &v->reg_cr0);
	host_json_parse_int_in(obj, len, "reg_cr1", &v->reg_cr1);
	host_json_parse_int_in(obj, len, "reg_cr2", &v->reg_cr2);
	host_json_parse_int_in(obj, len, "reg_cr_count", &v->reg_cr_count);
	host_json_parse_int_in(obj, len, "cpwm_orig", &v->cpwm_orig);
	host_json_parse_int_in(obj, len, "cpwm_after", &v->cpwm_after);
	host_json_parse_int_in(obj, len, "cpwm_reads_before_toggle", &v->cpwm_reads_before_toggle);
	host_json_parse_int_in(obj, len, "expect_h2c_count", &v->expect_h2c_count);
	if (!host_json_parse_int_in(obj, len, "expect_h2c_parm0", &v->expect_h2c_parm0))
		v->has_expect_h2c_parm0 = 1;
	host_json_parse_int_in(obj, len, "expect_btcoex_calls", &v->expect_btcoex_calls);
	host_json_parse_int_in(obj, len, "expect_btcoex_mode", &v->expect_btcoex_mode);
	host_json_parse_int_in(obj, len, "expect_min_read8", &v->expect_min_read8);
	host_json_parse_int_in(obj, len, "expect_hwreg_count_min", &v->expect_hwreg_count_min);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_pwrctrl_fw_ips_trace *tr;
	u8 hmetfr[8];
	u8 reg_cr[8];
	int nh = 0;
	int nr = 0;

	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.netif_up = (u8)v->netif_up_val;
	g_adapter.pwrctrlpriv.ips_mode_req = (u8)v->ips_mode_req;

	host_pwrctrl_fw_ips_reset_trace();
	host_pwrctrl_fw_ips_set_time_ms(0);

	if (v->hmetfr_count > 0) {
		hmetfr[nh++] = (u8)v->hmetfr0;
		if (v->hmetfr_count > 1)
			hmetfr[nh++] = (u8)v->hmetfr1;
		if (v->hmetfr_count > 2)
			hmetfr[nh++] = (u8)v->hmetfr2;
	}
	host_pwrctrl_fw_ips_set_hmetfr_sequence(hmetfr, nh);

	if (v->reg_cr_count > 0) {
		reg_cr[nr++] = (u8)v->reg_cr0;
		if (v->reg_cr_count > 1)
			reg_cr[nr++] = (u8)v->reg_cr1;
		if (v->reg_cr_count > 2)
			reg_cr[nr++] = (u8)v->reg_cr2;
	}
	host_pwrctrl_fw_ips_set_reg_cr_sequence(reg_cr, nr);

	host_pwrctrl_fw_ips_set_cpwm_leave((u8)v->cpwm_orig, (u8)v->cpwm_after,
					   (u32)v->cpwm_reads_before_toggle);

	rtw_set_fw_in_ips_mode(&g_adapter, (u8)v->enable);

	tr = host_pwrctrl_fw_ips_get_trace();
	if (tr->h2c_count != v->expect_h2c_count) {
		fprintf(stderr, "FAIL %s h2c_count=%d expect %d\n", v->name, tr->h2c_count,
			v->expect_h2c_count);
		return 1;
	}
	if (v->has_expect_h2c_parm0 && tr->h2c_count > 0 &&
	    tr->h2c[0].parm[0] != (u8)v->expect_h2c_parm0) {
		fprintf(stderr, "FAIL %s h2c parm0=%u expect %d\n", v->name, tr->h2c[0].parm[0],
			v->expect_h2c_parm0);
		return 1;
	}
	if (tr->btcoex_ips_notify_calls != v->expect_btcoex_calls) {
		fprintf(stderr, "FAIL %s btcoex_calls=%d expect %d\n", v->name,
			tr->btcoex_ips_notify_calls, v->expect_btcoex_calls);
		return 1;
	}
	if (v->expect_btcoex_calls && tr->last_btcoex_mode != (u8)v->expect_btcoex_mode) {
		fprintf(stderr, "FAIL %s btcoex_mode=%u expect %d\n", v->name, tr->last_btcoex_mode,
			v->expect_btcoex_mode);
		return 1;
	}
	if (tr->read8_count < v->expect_min_read8) {
		fprintf(stderr, "FAIL %s read8_count=%d min %d\n", v->name, tr->read8_count,
			v->expect_min_read8);
		return 1;
	}
	if (tr->hwreg_count < v->expect_hwreg_count_min) {
		fprintf(stderr, "FAIL %s hwreg_count=%d min %d\n", v->name, tr->hwreg_count,
			v->expect_hwreg_count_min);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t nvec;
	int i;
	int fails = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s vectors.json\n", argv[0]);
		return 2;
	}
	nvec = 0;
	if (host_load_vectors(argv[1], vecs, sizeof(struct vector), MAX_VECTORS, parse_vec,
			      &nvec)) {
		fprintf(stderr, "load vectors failed\n");
		return 2;
	}
	for (i = 0; i < (int)nvec; i++)
		fails += run_vec(&vecs[i]);
	if (fails)
		fprintf(stderr, "%d vector(s) failed\n", fails);
	else
		printf("OK %zu vectors\n", nvec);
	return fails ? 1 : 0;
}
