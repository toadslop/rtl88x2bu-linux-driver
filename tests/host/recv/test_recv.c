// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for recv_rest helpers (W3-39, W3-46).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_recv_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 48
#define MAX_NAME 128
#define MAX_ETH 32

enum recv_fn {
	FN_INC_CHK = 0,
	FN_RESET,
	FN_DEL_WFD,
	FN_LLC_PARSE,
	FN_WLAN_TO_ETH,
	FN_BMC_ALLOW,
	FN_RECV_DECACHE,
	FN_RECV_UCAST_PN,
	FN_RECV_BCAST_PN,
};

struct vector {
	char name[MAX_NAME];
	enum recv_fn fn;
	int tid_index;
	int initial_count;
	int expect_ret;
	int expect_count;
	u8 ies_offset;
	u8 frame[HOST_RECV_MAX_FRAME];
	size_t frame_len;
	unsigned int expect_len;
	u8 msdu[64];
	size_t msdu_len;
	int expect_llc;
	u8 hdrlen;
	u8 iv_len;
	u8 icv_len;
	u8 encrypt;
	u8 llc_hdl;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 expect_eth[MAX_ETH];
	size_t expect_eth_len;
	int adapter_fw_state;
	u8 adapter_linked;
	u8 expect_bmc;
	int priority;
	int qos;
	int seq_num;
	int frag_num;
	u8 ra[ETH_ALEN];
	u16 initial_seq;
	u16 expect_seq;
	u32 expect_dup_cnt;
	u8 initial_iv[8];
	u8 expect_iv[8];
	u8 initial_iv_seq[8];
	u8 expect_iv_seq[8];
	u8 ccmp_iv[8];
};

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);
bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset);
sint recv_decache(union recv_frame *precv_frame);
sint recv_ucast_pn_decache(union recv_frame *precv_frame);
sint recv_bcast_pn_decache(union recv_frame *precv_frame);

static int parse_fn(const char *obj, size_t obj_len, enum recv_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_inc_and_chk_continual_no_rx_packet"))
		*out = FN_INC_CHK;
	else if (!strcmp(fn, "rtw_reset_continual_no_rx_packet"))
		*out = FN_RESET;
	else if (!strcmp(fn, "rtw_rframe_del_wfd_ie"))
		*out = FN_DEL_WFD;
	else if (!strcmp(fn, "rtw_recv_llc_parse"))
		*out = FN_LLC_PARSE;
	else if (!strcmp(fn, "wlanhdr_to_ethhdr"))
		*out = FN_WLAN_TO_ETH;
	else if (!strcmp(fn, "adapter_allow_bmc_data_rx"))
		*out = FN_BMC_ALLOW;
	else if (!strcmp(fn, "recv_decache"))
		*out = FN_RECV_DECACHE;
	else if (!strcmp(fn, "recv_ucast_pn_decache"))
		*out = FN_RECV_UCAST_PN;
	else if (!strcmp(fn, "recv_bcast_pn_decache"))
		*out = FN_RECV_BCAST_PN;
	else
		return -1;
	return 0;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *out, size_t out_cap, size_t *out_len)
{
	char hex[HOST_RECV_MAX_FRAME * 2 + 1];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, out, out_cap, out_len);
}

static int parse_mac_field(const char *obj, size_t obj_len, const char *key, u8 *mac)
{
	char hex[ETH_ALEN * 2 + 1];
	size_t len = 0;

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, mac, ETH_ALEN, &len) || len != ETH_ALEN;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "tid_index", &v->tid_index);
	host_json_parse_int_in(obj, obj_len, "initial_count", &v->initial_count);
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "expect_count", &v->expect_count);
	if (v->fn == FN_DEL_WFD || v->fn == FN_WLAN_TO_ETH) {
		int ies_offset = 0;

		if (v->fn == FN_DEL_WFD) {
			host_json_parse_int_in(obj, obj_len, "ies_offset", &ies_offset);
			v->ies_offset = (u8)ies_offset;
		}
		if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame),
				    &v->frame_len))
			return -1;
		{
			int expect_len = 0;

			host_json_parse_int_in(obj, obj_len, "expect_len", &expect_len);
			v->expect_len = (unsigned int)expect_len;
		}
	}
	if (v->fn == FN_LLC_PARSE) {
		if (parse_hex_field(obj, obj_len, "msdu", v->msdu, sizeof(v->msdu),
				    &v->msdu_len))
			return -1;
		host_json_parse_int_in(obj, obj_len, "expect_llc", &v->expect_llc);
	}
	if (v->fn == FN_WLAN_TO_ETH) {
		int tmp;

		host_json_parse_int_in(obj, obj_len, "hdrlen", &tmp);
		v->hdrlen = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "iv_len", &tmp);
		v->iv_len = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "icv_len", &tmp);
		v->icv_len = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "encrypt", &tmp);
		v->encrypt = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "llc_hdl", &tmp);
		v->llc_hdl = (u8)tmp;
		if (parse_mac_field(obj, obj_len, "dst", v->dst) ||
		    parse_mac_field(obj, obj_len, "src", v->src))
			return -1;
		if (parse_hex_field(obj, obj_len, "expect_eth", v->expect_eth,
				    sizeof(v->expect_eth), &v->expect_eth_len))
			return -1;
	}
	if (v->fn == FN_BMC_ALLOW) {
		int linked = 0;
		int expect_bmc = 0;

		host_json_parse_int_in(obj, obj_len, "adapter_fw_state",
				       &v->adapter_fw_state);
		host_json_parse_int_in(obj, obj_len, "adapter_linked", &linked);
		v->adapter_linked = (u8)linked;
		host_json_parse_int_in(obj, obj_len, "expect_bmc", &expect_bmc);
		v->expect_bmc = (u8)expect_bmc;
	}
	if (v->fn == FN_RECV_DECACHE || v->fn == FN_RECV_UCAST_PN ||
	    v->fn == FN_RECV_BCAST_PN) {
		int tmp;
		size_t hex_len = 0;

		host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
		host_json_parse_int_in(obj, obj_len, "priority", &v->priority);
		host_json_parse_int_in(obj, obj_len, "qos", &tmp);
		v->qos = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "seq_num", &v->seq_num);
		host_json_parse_int_in(obj, obj_len, "frag_num", &tmp);
		v->frag_num = (u8)tmp;
		parse_mac_field(obj, obj_len, "ra", v->ra);
		host_json_parse_int_in(obj, obj_len, "initial_seq", &tmp);
		v->initial_seq = (u16)tmp;
		host_json_parse_int_in(obj, obj_len, "expect_seq", &tmp);
		v->expect_seq = (u16)tmp;
		host_json_parse_int_in(obj, obj_len, "expect_dup_cnt",
				       (int *)&v->expect_dup_cnt);
		host_json_parse_int_in(obj, obj_len, "hdrlen", &tmp);
		v->hdrlen = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "encrypt", &tmp);
		v->encrypt = (u8)tmp;
		host_json_parse_int_in(obj, obj_len, "adapter_fw_state",
				       &v->adapter_fw_state);
		parse_hex_field(obj, obj_len, "initial_iv", v->initial_iv,
				sizeof(v->initial_iv), &hex_len);
		parse_hex_field(obj, obj_len, "expect_iv", v->expect_iv,
				sizeof(v->expect_iv), &hex_len);
		parse_hex_field(obj, obj_len, "initial_iv_seq", v->initial_iv_seq,
				sizeof(v->initial_iv_seq), &hex_len);
		parse_hex_field(obj, obj_len, "expect_iv_seq", v->expect_iv_seq,
				sizeof(v->expect_iv_seq), &hex_len);
		parse_hex_field(obj, obj_len, "ccmp_iv", v->ccmp_iv,
				sizeof(v->ccmp_iv), &hex_len);
		if (v->fn == FN_RECV_DECACHE || v->fn == FN_RECV_UCAST_PN)
			parse_hex_field(obj, obj_len, "frame", v->frame,
					sizeof(v->frame), &v->frame_len);
	}
	return 0;
}

static u16 *decache_seq_slot(struct sta_info *sta, struct vector *v)
{
	sint tid = v->priority;

	if (v->qos) {
		if (v->ra[0] & 0x01)
			return &sta->sta_recvpriv.bmc_tid_rxseq[tid];
		return &sta->sta_recvpriv.rxcache.tid_rxseq[tid];
	}
	if (v->ra[0] & 0x01)
		return &sta->sta_recvpriv.nonqos_bmc_rxseq;
	return &sta->sta_recvpriv.nonqos_rxseq;
}

static int run_pn_vector(struct vector *v, sint (*fn)(union recv_frame *))
{
	struct sta_info sta;
	struct _adapter adapter;
	union recv_frame rframe;
	u8 frame_buf[HOST_RECV_MAX_FRAME];
	u16 *seq_slot;
	int ret;

	memset(&sta, 0, sizeof(sta));
	memset(&adapter, 0, sizeof(adapter));
	memset(&rframe, 0, sizeof(rframe));
	adapter.mlmepriv.fw_state = v->adapter_fw_state;
	sta.padapter = &adapter;
	rframe.u.hdr.adapter = &adapter;
	rframe.u.hdr.psta = &sta;
	rframe.u.hdr.attrib.priority = (u8)v->priority;
	rframe.u.hdr.attrib.qos = v->qos;
	rframe.u.hdr.attrib.seq_num = (u16)v->seq_num;
	rframe.u.hdr.attrib.frag_num = v->frag_num;
	memcpy(rframe.u.hdr.attrib.ra, v->ra, ETH_ALEN);
	rframe.u.hdr.attrib.hdrlen = v->hdrlen;
	rframe.u.hdr.attrib.encrypt = v->encrypt;

	seq_slot = decache_seq_slot(&sta, v);
	*seq_slot = v->initial_seq;

	if (v->fn == FN_RECV_UCAST_PN)
		memcpy(sta.sta_recvpriv.rxcache.iv[v->priority], v->initial_iv, 8);
	if (v->fn == FN_RECV_BCAST_PN)
		memcpy(adapter.securitypriv.iv_seq[0], v->initial_iv_seq, 8);

	memset(frame_buf, 0, sizeof(frame_buf));
	if (v->frame_len)
		memcpy(frame_buf, v->frame, v->frame_len);
	if (v->ccmp_iv[0] || v->ccmp_iv[1] || v->fn != FN_RECV_DECACHE)
		memcpy(frame_buf + v->hdrlen, v->ccmp_iv, 8);
	rframe.u.hdr.rx_data = frame_buf;

	ret = fn(&rframe);
	if (ret != v->expect_ret) {
		fprintf(stderr, "FAIL %s: expect_ret %d got %d\n", v->name,
			v->expect_ret, ret);
		return -1;
	}
	if (v->fn == FN_RECV_DECACHE && ret == _SUCCESS &&
	    *seq_slot != v->expect_seq) {
		fprintf(stderr, "FAIL %s: expect_seq %u got %u\n", v->name,
			v->expect_seq, *seq_slot);
		return -1;
	}
	if (v->fn == FN_RECV_DECACHE &&
	    sta.sta_stats.duplicate_cnt != v->expect_dup_cnt) {
		fprintf(stderr, "FAIL %s: expect_dup_cnt %u got %u\n", v->name,
			v->expect_dup_cnt, sta.sta_stats.duplicate_cnt);
		return -1;
	}
	if (v->fn == FN_RECV_UCAST_PN && ret == _SUCCESS &&
	    memcmp(sta.sta_recvpriv.rxcache.iv[v->priority], v->expect_iv, 8)) {
		fprintf(stderr, "FAIL %s: ucast iv mismatch\n", v->name);
		return -1;
	}
	if (v->fn == FN_RECV_BCAST_PN && ret == _SUCCESS &&
	    memcmp(adapter.securitypriv.iv_seq[0], v->expect_iv_seq, 8)) {
		fprintf(stderr, "FAIL %s: bcast iv_seq mismatch\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct sta_info sta;
	union recv_frame rframe;
	u8 frame_buf[HOST_RECV_MAX_FRAME];
	struct _adapter adapter;
	int ret;

	if (v->fn == FN_RECV_DECACHE)
		return run_pn_vector(v, recv_decache);
	if (v->fn == FN_RECV_UCAST_PN)
		return run_pn_vector(v, recv_ucast_pn_decache);
	if (v->fn == FN_RECV_BCAST_PN)
		return run_pn_vector(v, recv_bcast_pn_decache);

	if (v->fn == FN_LLC_PARSE) {
		ret = (int)rtw_recv_llc_parse(v->msdu, (u16)v->msdu_len);
		if (ret != v->expect_llc) {
			fprintf(stderr, "FAIL %s: expect_llc %d got %d\n", v->name,
				v->expect_llc, ret);
			return -1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	if (v->fn == FN_BMC_ALLOW) {
		memset(&adapter, 0, sizeof(adapter));
		adapter.mlmepriv.fw_state = v->adapter_fw_state;
		adapter.host_linked = v->adapter_linked;
		ret = (int)adapter_allow_bmc_data_rx(&adapter);
		if (ret != v->expect_bmc) {
			fprintf(stderr, "FAIL %s: expect_bmc %d got %d\n", v->name,
				v->expect_bmc, ret);
			return -1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	if (v->fn == FN_WLAN_TO_ETH) {
		memcpy(frame_buf, v->frame, v->frame_len);
		memset(&rframe, 0, sizeof(rframe));
		rframe.u.hdr.rx_head = frame_buf;
		rframe.u.hdr.rx_data = frame_buf;
		rframe.u.hdr.rx_tail = frame_buf + v->frame_len;
		rframe.u.hdr.rx_end = frame_buf + sizeof(frame_buf);
		rframe.u.hdr.len = (unsigned int)v->frame_len;
		rframe.u.hdr.attrib.hdrlen = v->hdrlen;
		rframe.u.hdr.attrib.iv_len = v->iv_len;
		rframe.u.hdr.attrib.icv_len = v->icv_len;
		rframe.u.hdr.attrib.encrypt = v->encrypt;
		memcpy(rframe.u.hdr.attrib.dst, v->dst, ETH_ALEN);
		memcpy(rframe.u.hdr.attrib.src, v->src, ETH_ALEN);

		ret = wlanhdr_to_ethhdr(&rframe, (enum rtw_rx_llc_hdl)v->llc_hdl);
		if (ret != v->expect_ret) {
			fprintf(stderr, "FAIL %s: expect_ret %d got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
		if (ret == _SUCCESS) {
			if (rframe.u.hdr.len != v->expect_len) {
				fprintf(stderr, "FAIL %s: expect_len %u got %u\n", v->name,
					v->expect_len, rframe.u.hdr.len);
				return -1;
			}
			if (memcmp(rframe.u.hdr.rx_data, v->expect_eth, v->expect_eth_len)) {
				fprintf(stderr, "FAIL %s: eth header mismatch\n", v->name);
				return -1;
			}
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	if (v->fn == FN_DEL_WFD) {
		memcpy(frame_buf, v->frame, v->frame_len);
		memset(&rframe, 0, sizeof(rframe));
		rframe.u.hdr.rx_data = frame_buf;
		rframe.u.hdr.len = (unsigned int)v->frame_len;

		ret = (int)rtw_rframe_del_wfd_ie(&rframe, v->ies_offset);
		if (ret != v->expect_ret) {
			fprintf(stderr, "FAIL %s: expect_ret %d got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
		if (rframe.u.hdr.len != v->expect_len) {
			fprintf(stderr, "FAIL %s: expect_len %u got %u\n", v->name,
				v->expect_len, rframe.u.hdr.len);
			return -1;
		}
		printf("PASS %s\n", v->name);
		return 0;
	}

	memset(&sta, 0, sizeof(sta));
	sta.continual_no_rx_packet[v->tid_index] = v->initial_count;

	if (v->fn == FN_INC_CHK) {
		ret = rtw_inc_and_chk_continual_no_rx_packet(&sta, v->tid_index);
		if (ret != v->expect_ret) {
			fprintf(stderr, "FAIL %s: expect_ret %d got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
	} else {
		rtw_reset_continual_no_rx_packet(&sta, v->tid_index);
	}
	if (sta.continual_no_rx_packet[v->tid_index] != v->expect_count) {
		fprintf(stderr, "FAIL %s: expect_count %d got %d\n", v->name,
			v->expect_count, sta.continual_no_rx_packet[v->tid_index]);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	int failures = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", argv[1]);
		return 2;
	}
	for (i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %d failures\n", count, failures);
	return failures ? 1 : 0;
}
