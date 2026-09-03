// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for mgnt frame attrib builders (W3-68). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_mlme_ext_mgnt_attrib_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_FRAME 128

enum mgnt_fn {
	FN_MONITOR = 0,
	FN_MGNT,
	FN_ADDR,
	FN_SUBTYPE,
};

struct expect_attrib {
	u8 set;
	u16 hdrlen;
	u8 nr_frags;
	u8 priority;
	u8 mac_id;
	u8 qsel;
	u32 pktlen;
	u8 raid;
	u8 rate;
	u8 encrypt;
	u8 bswenc;
	u8 qos_en;
	u8 ht_en;
	u8 bwmode;
	u8 ch_offset;
	u8 sgi;
	u16 seqnum;
	u8 retry_ctrl;
	u8 mbssid;
	u8 hw_ssn_sel;
	u8 ps_dontq;
	u8 subtype;
	u8 ra[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u16 txbf_p_aid;
	u16 txbf_g_id;
};

struct vector {
	char name[MAX_NAME];
	enum mgnt_fn fn;
	u8 tx_rate;
	u16 mgnt_seq;
	u8 hw_ssn_seq_no;
	u8 rf_type;
	u32 mlme_state;
	u8 frame_hex[MAX_FRAME];
	size_t frame_len;
	u32 pktlen;
	struct expect_attrib expect;
};

static _adapter adapter;
static struct sta_info fixture_sta;
static u8 frame_buf[MAX_FRAME + TXDESC_OFFSET];

static int parse_fn(const char *obj, size_t len, enum mgnt_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "update_monitor_frame_attrib"))
		*out = FN_MONITOR;
	else if (!strcmp(fn, "update_mgntframe_attrib"))
		*out = FN_MGNT;
	else if (!strcmp(fn, "update_mgntframe_attrib_addr"))
		*out = FN_ADDR;
	else if (!strcmp(fn, "update_mgntframe_subtype"))
		*out = FN_SUBTYPE;
	else
		return -1;
	return 0;
}

static int parse_expect(const char *obj, size_t len, struct expect_attrib *e)
{
	char hex[32];
	size_t decoded = 0;
	int tmp;

	memset(e, 0, sizeof(*e));
	e->set = 1;
	if (!host_json_parse_int_in(obj, len, "hdrlen", &tmp))
		e->hdrlen = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "nr_frags", &tmp))
		e->nr_frags = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "priority", &tmp))
		e->priority = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "mac_id", &tmp))
		e->mac_id = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "qsel", &tmp))
		e->qsel = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "pktlen", &tmp))
		e->pktlen = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "raid", &tmp))
		e->raid = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "rate", &tmp))
		e->rate = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "encrypt", &tmp))
		e->encrypt = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "bswenc", &tmp))
		e->bswenc = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "qos_en", &tmp))
		e->qos_en = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ht_en", &tmp))
		e->ht_en = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "bwmode", &tmp))
		e->bwmode = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ch_offset", &tmp))
		e->ch_offset = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "sgi", &tmp))
		e->sgi = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "seqnum", &tmp))
		e->seqnum = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "retry_ctrl", &tmp))
		e->retry_ctrl = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "mbssid", &tmp))
		e->mbssid = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "hw_ssn_sel", &tmp))
		e->hw_ssn_sel = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ps_dontq", &tmp))
		e->ps_dontq = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "subtype", &tmp))
		e->subtype = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "txbf_p_aid", &tmp))
		e->txbf_p_aid = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "txbf_g_id", &tmp))
		e->txbf_g_id = (u16)tmp;
	if (!host_json_parse_string_in(obj, len, "ra", hex, sizeof(hex)))
		host_hex_decode(hex, e->ra, ETH_ALEN, &decoded);
	if (!host_json_parse_string_in(obj, len, "ta", hex, sizeof(hex)))
		host_hex_decode(hex, e->ta, ETH_ALEN, &decoded);
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	if (!host_json_parse_int_in(obj, len, "tx_rate", &tmp))
		v->tx_rate = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "mgnt_seq", &tmp))
		v->mgnt_seq = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "hw_ssn_seq_no", &tmp))
		v->hw_ssn_seq_no = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "rf_type", &tmp))
		v->rf_type = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "mlme_state", &tmp))
		v->mlme_state = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "pktlen", &tmp))
		v->pktlen = (u32)tmp;
	if (!host_json_parse_string_in(obj, len, "frame_hex", hex, sizeof(hex))) {
		if (host_hex_decode(hex, v->frame_hex, sizeof(v->frame_hex), &decoded))
			return -1;
		v->frame_len = decoded;
	}
	return parse_expect(obj, len, &v->expect);
}

static void setup_adapter(struct vector *v)
{
	memset(&adapter, 0, sizeof(adapter));
	memset(&fixture_sta, 0, sizeof(fixture_sta));

	adapter.mlmeextpriv.tx_rate = v->tx_rate;
	adapter.mlmeextpriv.mgnt_seq = v->mgnt_seq;
	adapter.xmitpriv.hw_ssn_seq_no = v->hw_ssn_seq_no;
	adapter.hal_data.rf_type = v->rf_type;
	adapter.host_fixture.mlme_state = v->mlme_state;
	adapter.stapriv.fixture_sta = &fixture_sta;
	fixture_sta.cmn.bf_info.g_id = 7;
	fixture_sta.cmn.bf_info.p_aid = 42;
	host_mgnt_attrib_adapter = &adapter;
}

static int mac_eq(const u8 *a, const u8 *b)
{
	return memcmp(a, b, ETH_ALEN) == 0;
}

static int check_attrib(const char *name, struct pkt_attrib *a,
			struct expect_attrib *e)
{
#define CHK(field, fmt) \
	do { \
		if (a->field != e->field) { \
			fprintf(stderr, "%s: " #field " got " fmt " expect " fmt "\n", \
				name, a->field, e->field); \
			return -1; \
		} \
	} while (0)

	if (!e->set)
		return 0;

	CHK(hdrlen, "%u");
	CHK(nr_frags, "%u");
	CHK(priority, "%u");
	CHK(mac_id, "%u");
	CHK(qsel, "%u");
	CHK(pktlen, "%u");
	CHK(raid, "%u");
	CHK(rate, "%u");
	CHK(encrypt, "%u");
	CHK(bswenc, "%u");
	CHK(qos_en, "%u");
	CHK(ht_en, "%u");
	CHK(bwmode, "%u");
	CHK(ch_offset, "%u");
	CHK(sgi, "%u");
	CHK(seqnum, "%u");
	CHK(retry_ctrl, "%u");
	CHK(mbssid, "%u");
	CHK(hw_ssn_sel, "%u");
#ifdef CONFIG_RTW_MGMT_QUEUE
	CHK(ps_dontq, "%u");
	CHK(subtype, "%u");
#endif
#ifdef CONFIG_BEAMFORMING
	CHK(txbf_p_aid, "%u");
	CHK(txbf_g_id, "%u");
#endif
	if (e->ra[0] || e->ra[1] || e->ra[2] || e->ra[3] || e->ra[4] || e->ra[5]) {
		if (!mac_eq(a->ra, e->ra)) {
			fprintf(stderr, "%s: ra mismatch\n", name);
			return -1;
		}
	}
	if (e->ta[0] || e->ta[1] || e->ta[2] || e->ta[3] || e->ta[4] || e->ta[5]) {
		if (!mac_eq(a->ta, e->ta)) {
			fprintf(stderr, "%s: ta mismatch\n", name);
			return -1;
		}
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	struct pkt_attrib attrib;
	struct xmit_frame xframe;

	setup_adapter(v);
	memset(&attrib, 0, sizeof(attrib));
	memset(&xframe, 0, sizeof(xframe));

	switch (v->fn) {
	case FN_MONITOR:
		update_monitor_frame_attrib(&adapter, &attrib);
		break;
	case FN_MGNT:
		update_mgntframe_attrib(&adapter, &attrib);
		break;
	case FN_ADDR:
		memset(frame_buf, 0, sizeof(frame_buf));
		memcpy(frame_buf + TXDESC_OFFSET, v->frame_hex, v->frame_len);
		xframe.buf_addr = frame_buf;
		xframe.attrib = attrib;
		update_mgntframe_attrib_addr(&adapter, &xframe);
		return check_attrib(v->name, &xframe.attrib, &v->expect);
#ifdef CONFIG_RTW_MGMT_QUEUE
	case FN_SUBTYPE:
		memset(frame_buf, 0, sizeof(frame_buf));
		memcpy(frame_buf + TXDESC_OFFSET, v->frame_hex, v->frame_len);
		xframe.buf_addr = frame_buf;
		xframe.attrib.pktlen = v->pktlen;
		update_mgntframe_subtype(&adapter, &xframe);
		return check_attrib(v->name, &xframe.attrib, &v->expect);
#endif
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}

	return check_attrib(v->name, &attrib, &v->expect);
}

#ifndef RUST_MLME_EXT_MGNT_ATTRIB_ORACLE
/* C oracle uses functions from rtw_mlme_ext_rest.c */
#else
void update_monitor_frame_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
#ifdef CONFIG_RTW_MGMT_QUEUE
void update_mgntframe_subtype(_adapter *padapter, struct xmit_frame *pmgntframe);
#endif
void update_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe);
#endif

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0;
	size_t i;
	const char *path = (argc > 1) ? argv[1] : "mgnt_attrib_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(struct vector), MAX_VECTORS,
			      parse_vector_object, &n)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (i = 0; i < n; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			return 1;
		}
		printf("PASS: %s\n", vectors[i].name);
	}

	printf("All %zu vectors passed.\n", n);
	return 0;
}
