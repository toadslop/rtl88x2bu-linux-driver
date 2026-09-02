// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_restructure_ht_ie (W3-67). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_mlme_ht_restructure_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_IE 512

struct vector {
	char name[MAX_NAME];
	u8 in_ie[MAX_IE];
	size_t in_len;
	u8 use_null_in;
	u8 channel;
	u8 regsty_bw_2g;
	u8 regsty_bw_5g;
	u8 hal_bw_cap_40;
	u8 chset_allow_40;
	u8 chbw_non_ocp_40;
	u8 dfs_slave_with_rd;
	u8 dfs_domain_unknown;
	u8 fw_state;
	u8 cur_bwmode;
	u8 sgi_20m;
	u8 sgi_40m;
	u8 rx_nss;
	u8 beamform_cap;
	u8 beamformer_cap;
	u8 beamformee_cap;
	u8 expect_ht_option;
	u16 expect_cap_info_mask;
	u16 expect_cap_info_value;
	u32 expect_tx_bf_cap_info;
	u8 expect_tx_bf_cap_info_set;
	u8 expect_ht_info;
};

#ifndef RUST_MLME_HT_RESTRUCTURE_ORACLE
unsigned int rtw_restructure_ht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie,
				   uint in_len, uint *pout_len, u8 channel);
#else
unsigned int rtw_restructure_ht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie,
				   unsigned in_len, unsigned *pout_len, u8 channel);
#endif

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	v->channel = 6;
	v->regsty_bw_2g = 1;
	v->regsty_bw_5g = 1;
	v->hal_bw_cap_40 = 1;
	v->chset_allow_40 = 1;
	v->rx_nss = 2;
	v->expect_ht_option = 1;
	v->expect_cap_info_mask = 0xffff;
	v->fw_state = 0;

	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_string_in(obj, len, "in_ie_hex", hex, sizeof(hex))) {
		if (host_hex_decode(hex, v->in_ie, sizeof(v->in_ie), &decoded))
			return -1;
		v->in_len = decoded;
	}
	if (!host_json_parse_int_in(obj, len, "use_null_in", &tmp))
		v->use_null_in = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "channel", &tmp))
		v->channel = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "regsty_bw_2g", &tmp))
		v->regsty_bw_2g = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "regsty_bw_5g", &tmp))
		v->regsty_bw_5g = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "hal_bw_cap_40", &tmp))
		v->hal_bw_cap_40 = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "chset_allow_40", &tmp))
		v->chset_allow_40 = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "chbw_non_ocp_40", &tmp))
		v->chbw_non_ocp_40 = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "dfs_slave_with_rd", &tmp))
		v->dfs_slave_with_rd = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "dfs_domain_unknown", &tmp))
		v->dfs_domain_unknown = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "fw_state", &tmp))
		v->fw_state = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "cur_bwmode", &tmp))
		v->cur_bwmode = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "sgi_20m", &tmp))
		v->sgi_20m = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "sgi_40m", &tmp))
		v->sgi_40m = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "rx_nss", &tmp))
		v->rx_nss = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "beamform_cap", &tmp))
		v->beamform_cap = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "beamformer_cap", &tmp))
		v->beamformer_cap = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "beamformee_cap", &tmp))
		v->beamformee_cap = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_ht_option", &tmp))
		v->expect_ht_option = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_cap_info_mask", &tmp))
		v->expect_cap_info_mask = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_cap_info_value", &tmp))
		v->expect_cap_info_value = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_tx_bf_cap_info", &tmp)) {
		v->expect_tx_bf_cap_info = (u32)tmp;
		v->expect_tx_bf_cap_info_set = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_ht_info", &tmp))
		v->expect_ht_info = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	u8 out_ie[MAX_IE];
	uint out_len = 0;
	uint cap_len = 0;
	const u8 *ht_cap;
	u16 cap_info;

	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.bw_mode = (u8)((v->regsty_bw_5g << 4) | (v->regsty_bw_2g & 0x0f));
	adapter.host_fixture.hal_bw_cap_40 = v->hal_bw_cap_40;
	adapter.host_fixture.chset_allow_40 = v->chset_allow_40;
	adapter.host_fixture.chbw_non_ocp_40 = v->chbw_non_ocp_40;
	adapter.host_fixture.dfs_domain_unknown = v->dfs_domain_unknown;
	adapter.host_fixture.rx_nss = v->rx_nss;
	adapter.host_fixture.rx_packet_offset = 0;
	adapter.host_fixture.max_recvbuf_sz = 8192;
	adapter.host_fixture.max_rx_ampdu_factor = 3;
	adapter.mlmepriv.fw_state = v->fw_state;
	adapter.mlmeextpriv.cur_bwmode = v->cur_bwmode;
	adapter.mlmepriv.htpriv.sgi_20m = v->sgi_20m;
	adapter.mlmepriv.htpriv.sgi_40m = v->sgi_40m;
	adapter.mlmepriv.htpriv.beamform_cap = v->beamform_cap;
	adapter.host_fixture.beamformer_cap = v->beamformer_cap ? v->beamformer_cap : 2;
	adapter.host_fixture.beamformee_cap = v->beamformee_cap ? v->beamformee_cap : 2;
	adapter.rf_ctl.dfs_slave_with_rd = v->dfs_slave_with_rd;
	memset(adapter.mlmeextpriv.default_supported_mcs_set, 0xff, 16);
	host_mlme_ht_restructure_adapter = &adapter;

	if (rtw_restructure_ht_ie(&adapter, v->use_null_in ? NULL : (u8 *)v->in_ie,
				  out_ie, (uint)v->in_len, &out_len,
				  v->channel) != v->expect_ht_option) {
		fprintf(stderr, "%s: ht_option return mismatch\n", v->name);
		return -1;
	}
	if (adapter.mlmepriv.htpriv.ht_option != v->expect_ht_option) {
		fprintf(stderr, "%s: ht_option state mismatch\n", v->name);
		return -1;
	}
	if (!v->expect_ht_option)
		return 0;

	ht_cap = rtw_get_ie(out_ie, _HT_CAPABILITY_IE_, &cap_len, out_len);
	if (!ht_cap || cap_len != HT_CAP_IE_LEN) {
		fprintf(stderr, "%s: missing HT cap IE\n", v->name);
		return -1;
	}
	memcpy(&cap_info, ht_cap + 2, sizeof(cap_info));
	cap_info = (u16)(cap_info & v->expect_cap_info_mask);
	if (cap_info != v->expect_cap_info_value) {
		fprintf(stderr, "%s: cap_info mismatch got=0x%04x expect=0x%04x\n",
			v->name, cap_info, v->expect_cap_info_value);
		return -1;
	}
	if (v->expect_tx_bf_cap_info_set) {
		u32 tx_bf;

		memcpy(&tx_bf, ht_cap + 2 + 21, sizeof(tx_bf));
		if (tx_bf != v->expect_tx_bf_cap_info) {
			fprintf(stderr,
				"%s: tx_BF_cap_info mismatch got=0x%08x expect=0x%08x\n",
				v->name, tx_bf, v->expect_tx_bf_cap_info);
			return -1;
		}
	}
	if (v->expect_ht_info) {
		uint info_len = 0;
		const u8 *ht_info = rtw_get_ie(out_ie, _HT_ADD_INFO_IE_, &info_len, out_len);

		if (!ht_info || info_len != HT_OP_IE_LEN) {
			fprintf(stderr, "%s: missing HT info IE\n", v->name);
			return -1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "mlme_ht_restructure_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			return 1;
		}
	}
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
