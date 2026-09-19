// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_mlme_ext_join_cmd_types.h"
#include "host_vector_json.h"

extern void host_join_cmd_reset_trace(void);

static _adapter g_adapter;

struct vector {
	char name[64];
	int ielength, dsconfig, chk_fail, sta_mode, assoc_success;
	int wmm_ie, ht_cap_ie;
	int expect_ret, expect_report, expect_start_clnt, expect_wmm;
	int expect_ht_caps, expect_deauth;
};

static void init_fixed_ie(u8 *ies, u16 bcn_int)
{
	memset(ies, 0, 12);
	ies[8] = (u8)(bcn_int & 0xff);
	ies[9] = (u8)(bcn_int >> 8);
}

static void append_wmm_ie(u8 *ies, u32 *len)
{
	u8 *p = ies + *len;

	p[0] = _VENDOR_SPECIFIC_IE_;
	p[1] = 7;
	memcpy(p + 2, WMM_OUI, 4);
	p[6] = 0;
	p[7] = 0;
	p[8] = 0;
	*len += 9;
}

static void append_ht_cap_ie(u8 *ies, u32 *len)
{
	u8 *p = ies + *len;

	p[0] = _HT_CAPABILITY_IE_;
	p[1] = 26;
	memset(p + 2, 0, 26);
	*len += 28;
}

static void build_join_buf(WLAN_BSSID_EX *b, struct vector *v)
{
	u32 ie_len = 0;

	memset(b, 0, sizeof(*b));
	b->Configuration.DSConfig = (u32)v->dsconfig;
	if ((u32)v->ielength > MAX_IE_SZ) {
		b->IELength = (u32)v->ielength;
		return;
	}
	if ((u32)v->ielength > 0 && (u32)v->ielength < 12) {
		b->IELength = (u32)v->ielength;
		return;
	}
	init_fixed_ie(b->IEs, 100);
	ie_len = 12;
	if (v->wmm_ie)
		append_wmm_ie(b->IEs, &ie_len);
	if (v->ht_cap_ie)
		append_ht_cap_ie(b->IEs, &ie_len);
	if ((u32)v->ielength > ie_len)
		ie_len = (u32)v->ielength;
	b->IELength = ie_len;
}

static int parse_vec(const char *o, size_t l, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(o, l, "name", v->name, sizeof(v->name)))
		return -1;
#define I(f, fld) host_json_parse_int_in(o, l, f, &v->fld)
	I("ielength", ielength);
	I("dsconfig", dsconfig);
	I("chk_fail", chk_fail);
	I("sta_mode", sta_mode);
	I("assoc_success", assoc_success);
	I("wmm_ie", wmm_ie);
	I("ht_cap_ie", ht_cap_ie);
	I("expect_ret", expect_ret);
	I("expect_report", expect_report);
	I("expect_start_clnt", expect_start_clnt);
	I("expect_wmm", expect_wmm);
	I("expect_ht_caps", expect_ht_caps);
	I("expect_deauth", expect_deauth);
#undef I
	if (!v->dsconfig)
		v->dsconfig = 6;
	return 0;
}

static int run_vector(struct vector *v)
{
	WLAN_BSSID_EX parm;
	u8 ret;
	struct mlme_ext_info *info;

	host_join_cmd_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.mlmepriv.fw_state = v->sta_mode ? WIFI_STATION_STATE : 0;
	info = &g_adapter.mlmeextpriv.mlmext_info;
	if (v->assoc_success)
		info->state = WIFI_FW_ASSOC_SUCCESS | WIFI_FW_STATION_STATE;
	host_chk_start_clnt_join_result = v->chk_fail ? _FAIL : _SUCCESS;

	build_join_buf(&parm, v);
	ret = join_cmd_hdl(&g_adapter, (u8 *)&parm);

	if ((int)ret != v->expect_ret)
		return -1;
	if (!!host_join_cmd_trace.report_join_res_called != !!v->expect_report)
		return -1;
	if (!!host_join_cmd_trace.start_clnt_join != !!v->expect_start_clnt)
		return -1;
	if (!!host_join_cmd_trace.wmm_handler != !!v->expect_wmm)
		return -1;
	if (!!info->HT_caps_enable != !!v->expect_ht_caps)
		return -1;
	if (!!host_join_cmd_trace.deauth_called != !!v->expect_deauth)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	const char *path = argc > 1 ? argv[1] : "mlme_ext_join_cmd_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), sizeof(vecs) / sizeof(vecs[0]),
			      parse_vec, &n))
		return 1;

	for (size_t i = 0; i < n; i++)
		if (run_vector(&vecs[i]))
			return 1;

#ifndef RUST_MLME_EXT_JOIN_CMD_ORACLE
	printf("PASS %zu vectors (oracle: core/rtw_mlme_ext_rest.c) (%s)\n", n, path);
#else
	printf("PASS %zu vectors (oracle: rust/rtw_mlme_ext_join_cmd.rs) (%s)\n", n, path);
#endif
	return 0;
}
