// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_hal_update_sta_smps_rate_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8

struct vector {
	char name[40];
	int kind;
	u8 mac_id, expect_sm_ps;
	u8 ht_enable, wireless_mode, ap_state;
	s8 hal_tx_nss;
	u8 ht_option, smps_cap, vht_option;
	u8 supp_mcs_set[HT_MCS_SET_LEN];
	u8 vht_mcs_map[VHT_MCS_MAP_LEN];
	u8 bssrateset[BSS_RATES_LEN];
	u64 expect_ramask;
	u8 expect_init_rate;
};

static int hex_bytes(const char *hex, u8 *out, size_t cap)
{
	size_t n = strlen(hex) / 2;
	unsigned v;

	if (n > cap)
		return -1;
	for (size_t i = 0; i < n; i++) {
		if (sscanf(hex + i * 2, "%2x", &v) != 1)
			return -1;
		out[i] = (u8)v;
	}
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	char hx[96];
	int t;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "kind", &t))
		return -1;
	v->kind = t;
	if (v->kind == KIND_SMPS) {
		if (host_json_parse_int_in(obj, len, "mac_id", &t))
			return -1;
		v->mac_id = (u8)t;
		if (host_json_parse_int_in(obj, len, "expect_sm_ps", &t))
			return -1;
		v->expect_sm_ps = (u8)t;
		return 0;
	}
#define PI(f, dst) do { if (host_json_parse_int_in(obj, len, f, &t)) return -1; dst = (u8)t; } while (0)
	PI("ht_enable", v->ht_enable);
	PI("wireless_mode", v->wireless_mode);
	PI("hal_tx_nss", v->hal_tx_nss);
	PI("ap_state", v->ap_state);
	if (!host_json_parse_string_in(obj, len, "bssrateset", hx, sizeof(hx)))
		hex_bytes(hx, v->bssrateset, BSS_RATES_LEN);
	if (!host_json_parse_int_in(obj, len, "ht_option", &t))
		v->ht_option = (u8)t;
	if (!host_json_parse_int_in(obj, len, "smps_cap", &t))
		v->smps_cap = (u8)t;
	if (!host_json_parse_string_in(obj, len, "supp_mcs_set", hx, sizeof(hx)))
		hex_bytes(hx, v->supp_mcs_set, HT_MCS_SET_LEN);
	if (!host_json_parse_int_in(obj, len, "vht_option", &t))
		v->vht_option = (u8)t;
	if (!host_json_parse_string_in(obj, len, "vht_mcs_map", hx, sizeof(hx)))
		hex_bytes(hx, v->vht_mcs_map, VHT_MCS_MAP_LEN);
	if (host_json_parse_string_in(obj, len, "expect_ramask", hx, sizeof(hx)))
		return -1;
	if (sscanf(hx, "%llx", (unsigned long long *)&v->expect_ramask) != 1)
		return -1;
	return host_json_parse_int_in(obj, len, "expect_init_rate", &t)
	       ? -1
	       : (v->expect_init_rate = (u8)t, 0);
#undef PI
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	const char *path = argc > 1 ? argv[1] : "hal_update_sta_smps_rate_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return fputs("load failed\n", stderr), 1;
	for (size_t i = 0; i < n; i++) {
		struct host_adapter_smps_rate a;
		struct host_sta_smps_rate s;

		memset(&a, 0, sizeof(a));
		memset(&s, 0, sizeof(s));
		if (vecs[i].kind == KIND_SMPS) {
			s.mac_id = vecs[i].mac_id;
			rtw_hal_update_sta_smps_cap(&a, &s);
			if (s.sm_ps != vecs[i].expect_sm_ps)
				return fprintf(stderr, "FAIL %s sm_ps\n", vecs[i].name), 1;
			continue;
		}
		a.ht_enable = vecs[i].ht_enable;
		a.wireless_mode = vecs[i].wireless_mode;
		a.hal_tx_nss = vecs[i].hal_tx_nss;
		a.ap_state = vecs[i].ap_state;
		s.ht_option = vecs[i].ht_option;
		s.smps_cap = vecs[i].smps_cap;
		s.vht_option = vecs[i].vht_option;
		memcpy(s.supp_mcs_set, vecs[i].supp_mcs_set, HT_MCS_SET_LEN);
		memcpy(s.vht_mcs_map, vecs[i].vht_mcs_map, VHT_MCS_MAP_LEN);
		memcpy(s.bssrateset, vecs[i].bssrateset, BSS_RATES_LEN);
		rtw_hal_update_sta_rate_mask(&a, &s);
		if (s.ramask != vecs[i].expect_ramask || s.init_rate != vecs[i].expect_init_rate)
			return fprintf(stderr, "FAIL %s rate mask\n", vecs[i].name), 1;
	}
	printf("PASS: %zu vectors\n", n);
	return 0;
}