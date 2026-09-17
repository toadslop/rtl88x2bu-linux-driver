// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_sta_info_types.h"
#include "host_vector_json.h"

static u8 g_last_min_space;
static u8 g_last_ampdu_factor;
static int g_hwreg_calls;

void rtw_hal_set_hwreg(_adapter *padapter, enum hw_var variable, u8 *val)
{
	(void)padapter;
	g_hwreg_calls++;
	if (variable == HW_VAR_AMPDU_MIN_SPACE && val)
		g_last_min_space = *val;
	else if (variable == HW_VAR_AMPDU_FACTOR && val)
		g_last_ampdu_factor = *val;
}

struct vector {
	char name[64];
	u8 ampdu_para;
	u16 ht_caps_info;
	u8 expect_min_space;
	u8 expect_ampdu_factor;
	u8 expect_sm_ps;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ampdu_para", &tmp))
		return -1;
	v->ampdu_para = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "ht_caps_info", &tmp))
		return -1;
	v->ht_caps_info = (u16)tmp;
	if (host_json_parse_int_in(obj, len, "expect_min_space", &tmp))
		return -1;
	v->expect_min_space = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_ampdu_factor", &tmp))
		return -1;
	v->expect_ampdu_factor = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_sm_ps", &tmp))
		return -1;
	v->expect_sm_ps = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;

	memset(&adapter, 0, sizeof(adapter));
	g_last_min_space = 0xff;
	g_last_ampdu_factor = 0xff;
	g_hwreg_calls = 0;
	adapter.mlmeextpriv.mlmext_info.HT_caps.u.HT_cap_element.AMPDU_para = v->ampdu_para;
	adapter.mlmeextpriv.mlmext_info.HT_caps.u.HT_cap_element.HT_caps_info = v->ht_caps_info;

	update_hw_ht_param(&adapter);

	if (g_hwreg_calls != 2) {
		fprintf(stderr, "FAIL %s: expected 2 hwreg calls, got %d\n", v->name, g_hwreg_calls);
		return -1;
	}
	if (g_last_min_space != v->expect_min_space ||
	    g_last_ampdu_factor != v->expect_ampdu_factor) {
		fprintf(stderr,
			"FAIL %s: hwreg min_space=0x%02x factor=0x%02x expect 0x%02x 0x%02x\n",
			v->name, g_last_min_space, g_last_ampdu_factor,
			v->expect_min_space, v->expect_ampdu_factor);
		return -1;
	}
	if (adapter.mlmeextpriv.mlmext_info.SM_PS != v->expect_sm_ps) {
		fprintf(stderr, "FAIL %s: SM_PS=0x%02x expect 0x%02x\n",
			v->name, adapter.mlmeextpriv.mlmext_info.SM_PS, v->expect_sm_ps);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
