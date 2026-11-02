// SPDX-License-Identifier: GPL-2.0
/* W3-200 L2 runner: init store_u8 wowlan priv fields. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_init.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	int field;
	int val;
	int has_expect_wowlan_mode;
	int expect_wowlan_mode;
	int has_expect_wowlan_ap_mode;
	int expect_wowlan_ap_mode;
	int has_expect_wowlan_p2p_mode;
	int expect_wowlan_p2p_mode;
	int has_expect_wowlan_in_resume;
	int expect_wowlan_in_resume;
	int has_expect_wowlan_last_wake_reason;
	int expect_wowlan_last_wake_reason;
	int has_expect_wowlan_power_mgmt;
	int expect_wowlan_power_mgmt;
	int has_expect_wowlan_lps_level;
	int expect_wowlan_lps_level;
	int has_expect_default_patterns_en;
	int expect_default_patterns_en;
	int has_expect_wowlan_aoac_rpt_loc;
	int expect_wowlan_aoac_rpt_loc;
	int has_expect_rpwm_retry;
	int expect_rpwm_retry;
};

extern PADAPTER init_store_wowlan_test_adapter(void);
extern struct pwrctrl_priv_init *init_store_wowlan_test_pwr(PADAPTER padapter);
extern void init_store_wowlan_oracle_store_u8(PADAPTER padapter, u32 field, u8 val);

#ifdef HOST_PWRCTRL_INIT_STORE_WOWLAN_RUST
extern void rtw_rust_init_store_u8_wowlan(PADAPTER padapter, u32 field, u8 val);
#endif

static int parse_vector(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "field", &v->field))
		return -1;
	if (host_json_parse_int_in(obj, len, "val", &v->val))
		return -1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_mode", &v->expect_wowlan_mode))
		v->has_expect_wowlan_mode = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_ap_mode", &v->expect_wowlan_ap_mode))
		v->has_expect_wowlan_ap_mode = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_p2p_mode", &v->expect_wowlan_p2p_mode))
		v->has_expect_wowlan_p2p_mode = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_in_resume", &v->expect_wowlan_in_resume))
		v->has_expect_wowlan_in_resume = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_last_wake_reason",
				    &v->expect_wowlan_last_wake_reason))
		v->has_expect_wowlan_last_wake_reason = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_power_mgmt", &v->expect_wowlan_power_mgmt))
		v->has_expect_wowlan_power_mgmt = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_lps_level", &v->expect_wowlan_lps_level))
		v->has_expect_wowlan_lps_level = 1;
	if (!host_json_parse_int_in(obj, len, "expect_default_patterns_en",
				    &v->expect_default_patterns_en))
		v->has_expect_default_patterns_en = 1;
	if (!host_json_parse_int_in(obj, len, "expect_wowlan_aoac_rpt_loc",
				    &v->expect_wowlan_aoac_rpt_loc))
		v->has_expect_wowlan_aoac_rpt_loc = 1;
	if (!host_json_parse_int_in(obj, len, "expect_rpwm_retry", &v->expect_rpwm_retry))
		v->has_expect_rpwm_retry = 1;
	return 0;
}

static void call_store(PADAPTER pad, int field, int val)
{
#ifdef HOST_PWRCTRL_INIT_STORE_WOWLAN_RUST
	rtw_rust_init_store_u8_wowlan(pad, (u32)field, (u8)val);
#else
	init_store_wowlan_oracle_store_u8(pad, (u32)field, (u8)val);
#endif
}

static int check_field(struct vector *v, struct pwrctrl_priv_init *p)
{
	if (v->has_expect_wowlan_mode && (int)p->wowlan_mode != v->expect_wowlan_mode) {
		fprintf(stderr, "FAIL %s wowlan_mode=%u expect %d\n", v->name, p->wowlan_mode,
			v->expect_wowlan_mode);
		return -1;
	}
	if (v->has_expect_wowlan_ap_mode && (int)p->wowlan_ap_mode != v->expect_wowlan_ap_mode) {
		fprintf(stderr, "FAIL %s wowlan_ap_mode=%u expect %d\n", v->name, p->wowlan_ap_mode,
			v->expect_wowlan_ap_mode);
		return -1;
	}
	if (v->has_expect_wowlan_p2p_mode && (int)p->wowlan_p2p_mode != v->expect_wowlan_p2p_mode) {
		fprintf(stderr, "FAIL %s wowlan_p2p_mode=%u expect %d\n", v->name, p->wowlan_p2p_mode,
			v->expect_wowlan_p2p_mode);
		return -1;
	}
	if (v->has_expect_wowlan_in_resume && (int)p->wowlan_in_resume != v->expect_wowlan_in_resume) {
		fprintf(stderr, "FAIL %s wowlan_in_resume=%u expect %d\n", v->name, p->wowlan_in_resume,
			v->expect_wowlan_in_resume);
		return -1;
	}
	if (v->has_expect_wowlan_last_wake_reason &&
	    (int)p->wowlan_last_wake_reason != v->expect_wowlan_last_wake_reason) {
		fprintf(stderr, "FAIL %s wowlan_last_wake_reason=%u expect %d\n", v->name,
			p->wowlan_last_wake_reason, v->expect_wowlan_last_wake_reason);
		return -1;
	}
	if (v->has_expect_wowlan_power_mgmt &&
	    (int)p->wowlan_power_mgmt != v->expect_wowlan_power_mgmt) {
		fprintf(stderr, "FAIL %s wowlan_power_mgmt=%u expect %d\n", v->name,
			p->wowlan_power_mgmt, v->expect_wowlan_power_mgmt);
		return -1;
	}
	if (v->has_expect_wowlan_lps_level &&
	    (int)p->wowlan_lps_level != v->expect_wowlan_lps_level) {
		fprintf(stderr, "FAIL %s wowlan_lps_level=%u expect %d\n", v->name, p->wowlan_lps_level,
			v->expect_wowlan_lps_level);
		return -1;
	}
	if (v->has_expect_default_patterns_en &&
	    (int)p->default_patterns_en != v->expect_default_patterns_en) {
		fprintf(stderr, "FAIL %s default_patterns_en=%u expect %d\n", v->name,
			p->default_patterns_en, v->expect_default_patterns_en);
		return -1;
	}
	if (v->has_expect_wowlan_aoac_rpt_loc &&
	    (int)p->wowlan_aoac_rpt_loc != v->expect_wowlan_aoac_rpt_loc) {
		fprintf(stderr, "FAIL %s wowlan_aoac_rpt_loc=%u expect %d\n", v->name,
			p->wowlan_aoac_rpt_loc, v->expect_wowlan_aoac_rpt_loc);
		return -1;
	}
	if (v->has_expect_rpwm_retry && (int)p->rpwm_retry != v->expect_rpwm_retry) {
		fprintf(stderr, "FAIL %s rpwm_retry=%u expect %d\n", v->name, p->rpwm_retry,
			v->expect_rpwm_retry);
		return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	PADAPTER pad = init_store_wowlan_test_adapter();
	struct pwrctrl_priv_init *p;

	if (!pad)
		return -1;
	call_store(pad, v->field, v->val);
	p = init_store_wowlan_test_pwr(pad);
	return check_field(v, p);
}

int main(int argc, char **argv)
{
	const char *path = (argc > 1) ? argv[1] : "init_store_wowlan_vectors.json";
	struct vector vecs[32];
	size_t n = 0;
	int fails = 0;

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vector, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		fails += run_vector(&vecs[i]) != 0;
	if (fails) {
		fprintf(stderr, "FAIL %d/%zu (%s)\n", fails, n, path);
		return 1;
	}
	printf("PASS %zu vectors (%s)\n", n, path);
	return 0;
}
