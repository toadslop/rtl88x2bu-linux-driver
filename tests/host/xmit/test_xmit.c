// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_xmit_rest helpers (W3-40).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_xmit_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum xmit_fn {
	FN_TX_BW_MODE = 0,
	FN_ADAPTER_RATE_BMP,
	FN_SHARED_RATE_BMP,
	FN_HT_BW_BMP,
	FN_VHT_BW_BMP,
	FN_ADAPTER_AGG_RATE_BMP,
	FN_QUERY_RA_SHORT_GI,
};

struct vector {
	char name[MAX_NAME];
	enum xmit_fn fn;
	int sta_bw;
	int fw_state;
	int cur_channel;
	int driver_tx_bw_mode;
	int iface_id;
	int bw;
	int rate;
	int max_bw;
	int macid_id;
	int macid_num;
	int macid_iface0;
	int macid_iface1;
	int macid_bw;
	int macid_vht_en;
	unsigned int rate_bmp0;
	unsigned int rate_bmp1;
	unsigned int ht_bmp_20;
	unsigned int ht_bmp_40;
	unsigned long long vht_bmp_20;
	unsigned long long vht_bmp_40;
	unsigned long long vht_bmp_80;
	int expect_u8;
	int expect_cck_ofdm;
	unsigned int expect_ht;
	unsigned long long expect_vht;
	int expect_cck_ofdm_20;
	unsigned int expect_ht_20;
	unsigned int expect_ht_40;
	unsigned long long expect_vht_20;
	unsigned long long expect_vht_40;
	unsigned long long expect_vht_80;
	unsigned long long expect_vht_160;
	int sgi_20m;
	int sgi_40m;
	int sgi_80m;
	int vht_option;
	int hal_bw_cap;
};

static int parse_fn(const char *obj, size_t obj_len, enum xmit_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_get_tx_bw_mode"))
		*out = FN_TX_BW_MODE;
	else if (!strcmp(fn, "rtw_get_adapter_tx_rate_bmp_by_bw"))
		*out = FN_ADAPTER_RATE_BMP;
	else if (!strcmp(fn, "rtw_get_shared_macid_tx_rate_bmp_by_bw"))
		*out = FN_SHARED_RATE_BMP;
	else if (!strcmp(fn, "rtw_get_tx_bw_bmp_of_ht_rate"))
		*out = FN_HT_BW_BMP;
	else if (!strcmp(fn, "rtw_get_tx_bw_bmp_of_vht_rate"))
		*out = FN_VHT_BW_BMP;
	else if (!strcmp(fn, "rtw_get_adapter_tx_rate_bmp"))
		*out = FN_ADAPTER_AGG_RATE_BMP;
	else if (!strcmp(fn, "query_ra_short_GI"))
		*out = FN_QUERY_RA_SHORT_GI;
	else
		return -1;
	return 0;
}

static int parse_ull(const char *obj, size_t obj_len, const char *key,
		     unsigned long long *out)
{
	char buf[64];
	char *end;

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf))) {
		int iv = 0;

		if (host_json_parse_int_in(obj, obj_len, key, &iv))
			return -1;
		*out = (unsigned long long)iv;
		return 0;
	}
	*out = strtoull(buf, &end, 0);
	return 0;
}

static void setup_macid(struct macid_ctl_t *ctl, struct vector *v)
{
	u8 id = (u8)v->macid_id;

	memset(ctl, 0, sizeof(*ctl));
	ctl->num = v->macid_num ? (u8)v->macid_num : MACID_NUM_SW_LIMIT;
	if (id >= 32)
		return;
	ctl->used.m0 |= BIT(id);
	ctl->bw[id] = (u8)v->macid_bw;
	ctl->vht_en[id] = (u8)v->macid_vht_en;
	ctl->rate_bmp0[id] = v->rate_bmp0;
	ctl->rate_bmp1[id] = v->rate_bmp1;
	if (v->macid_iface0 >= 0 && v->macid_iface0 < CONFIG_IFACE_NUMBER)
		ctl->if_g[v->macid_iface0].m0 |= BIT(id);
	if (v->macid_iface1 >= 0 && v->macid_iface1 < CONFIG_IFACE_NUMBER)
		ctl->if_g[v->macid_iface1].m0 |= BIT(id);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	v->macid_iface0 = -1;
	v->macid_iface1 = -1;
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "sta_bw", &v->sta_bw);
	host_json_parse_int_in(obj, obj_len, "fw_state", &v->fw_state);
	host_json_parse_int_in(obj, obj_len, "cur_channel", &v->cur_channel);
	host_json_parse_int_in(obj, obj_len, "driver_tx_bw_mode", &v->driver_tx_bw_mode);
	host_json_parse_int_in(obj, obj_len, "iface_id", &v->iface_id);
	host_json_parse_int_in(obj, obj_len, "bw", &v->bw);
	host_json_parse_int_in(obj, obj_len, "rate", &v->rate);
	host_json_parse_int_in(obj, obj_len, "max_bw", &v->max_bw);
	host_json_parse_int_in(obj, obj_len, "macid_id", &v->macid_id);
	host_json_parse_int_in(obj, obj_len, "macid_num", &v->macid_num);
	host_json_parse_int_in(obj, obj_len, "macid_iface0", &v->macid_iface0);
	host_json_parse_int_in(obj, obj_len, "macid_iface1", &v->macid_iface1);
	host_json_parse_int_in(obj, obj_len, "macid_bw", &v->macid_bw);
	host_json_parse_int_in(obj, obj_len, "macid_vht_en", &v->macid_vht_en);
	{
		int tmp = 0;

		if (!host_json_parse_int_in(obj, obj_len, "rate_bmp0", &tmp))
			v->rate_bmp0 = (unsigned int)tmp;
		if (!host_json_parse_int_in(obj, obj_len, "rate_bmp1", &tmp))
			v->rate_bmp1 = (unsigned int)tmp;
		if (!host_json_parse_int_in(obj, obj_len, "ht_bmp_20", &tmp))
			v->ht_bmp_20 = (unsigned int)tmp;
		if (!host_json_parse_int_in(obj, obj_len, "ht_bmp_40", &tmp))
			v->ht_bmp_40 = (unsigned int)tmp;
	}
	parse_ull(obj, obj_len, "vht_bmp_20", &v->vht_bmp_20);
	parse_ull(obj, obj_len, "vht_bmp_40", &v->vht_bmp_40);
	parse_ull(obj, obj_len, "vht_bmp_80", &v->vht_bmp_80);
	host_json_parse_int_in(obj, obj_len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, obj_len, "expect_cck_ofdm", &v->expect_cck_ofdm);
	{
		int tmp = 0;

		if (!host_json_parse_int_in(obj, obj_len, "expect_ht", &tmp))
			v->expect_ht = (unsigned int)tmp;
	}
	parse_ull(obj, obj_len, "expect_vht", &v->expect_vht);
	host_json_parse_int_in(obj, obj_len, "expect_cck_ofdm_20", &v->expect_cck_ofdm_20);
	{
		int tmp = 0;

		if (!host_json_parse_int_in(obj, obj_len, "expect_ht_20", &tmp))
			v->expect_ht_20 = (unsigned int)tmp;
		if (!host_json_parse_int_in(obj, obj_len, "expect_ht_40", &tmp))
			v->expect_ht_40 = (unsigned int)tmp;
	}
	parse_ull(obj, obj_len, "expect_vht_20", &v->expect_vht_20);
	parse_ull(obj, obj_len, "expect_vht_40", &v->expect_vht_40);
	parse_ull(obj, obj_len, "expect_vht_80", &v->expect_vht_80);
	parse_ull(obj, obj_len, "expect_vht_160", &v->expect_vht_160);
	host_json_parse_int_in(obj, obj_len, "sgi_20m", &v->sgi_20m);
	host_json_parse_int_in(obj, obj_len, "sgi_40m", &v->sgi_40m);
	host_json_parse_int_in(obj, obj_len, "sgi_80m", &v->sgi_80m);
	host_json_parse_int_in(obj, obj_len, "vht_option", &v->vht_option);
	host_json_parse_int_in(obj, obj_len, "hal_bw_cap", &v->hal_bw_cap);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct dvobj_priv dvobj;
	_adapter adapter;
	struct sta_info sta;
	u16 cck = 0;
	u32 ht = 0;
	u64 vht = 0;
	u8 got;

	memset(&dvobj, 0, sizeof(dvobj));
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.dvobj = &dvobj;
	adapter.mlmepriv.fw_state = v->fw_state;
	adapter.mlmeextpriv.cur_channel = (u8)v->cur_channel;
	adapter.driver_tx_bw_mode = (u8)v->driver_tx_bw_mode;
	adapter.iface_id = (u8)v->iface_id;
	adapter.fix_rate = 0xFF;
	adapter.fix_bw = 0xFF;
	adapter.hal_bw_cap = v->hal_bw_cap ? (u8)v->hal_bw_cap :
		(BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M);
	sta.cmn.bw_mode = (u8)v->sta_bw;
	setup_macid(&dvobj.macid_ctl, v);
	dvobj.rf_ctl.rate_bmp_ht_by_bw[CHANNEL_WIDTH_20] = v->ht_bmp_20;
	dvobj.rf_ctl.rate_bmp_ht_by_bw[CHANNEL_WIDTH_40] = v->ht_bmp_40;
	dvobj.rf_ctl.rate_bmp_vht_by_bw[CHANNEL_WIDTH_20] = v->vht_bmp_20;
	dvobj.rf_ctl.rate_bmp_vht_by_bw[CHANNEL_WIDTH_40] = v->vht_bmp_40;
	dvobj.rf_ctl.rate_bmp_vht_by_bw[CHANNEL_WIDTH_80] = v->vht_bmp_80;

	switch (v->fn) {
	case FN_TX_BW_MODE:
		got = rtw_get_tx_bw_mode(&adapter, &sta);
		if (got != (u8)v->expect_u8) {
			fprintf(stderr, "FAIL %s: got bw %u expect %d\n", v->name, got,
				v->expect_u8);
			return -1;
		}
		break;
	case FN_ADAPTER_RATE_BMP:
		rtw_get_adapter_tx_rate_bmp_by_bw(&adapter, (u8)v->bw, &cck, &ht, &vht);
		if (cck != (u16)v->expect_cck_ofdm || ht != v->expect_ht ||
		    vht != v->expect_vht) {
			fprintf(stderr,
				"FAIL %s: got cck=0x%x ht=0x%x vht=0x%llx expect cck=0x%x ht=0x%x vht=0x%llx\n",
				v->name, cck, ht, (unsigned long long)vht,
				v->expect_cck_ofdm, v->expect_ht,
				(unsigned long long)v->expect_vht);
			return -1;
		}
		break;
	case FN_SHARED_RATE_BMP:
		rtw_get_shared_macid_tx_rate_bmp_by_bw(&dvobj, (u8)v->bw, &cck, &ht,
						       &vht);
		if (cck != (u16)v->expect_cck_ofdm || ht != v->expect_ht ||
		    vht != v->expect_vht) {
			fprintf(stderr, "FAIL %s: shared bmp mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_HT_BW_BMP:
		got = rtw_get_tx_bw_bmp_of_ht_rate(&dvobj, (u8)v->rate, (u8)v->max_bw);
		if (got != (u8)v->expect_u8) {
			fprintf(stderr, "FAIL %s: ht bw bmp got %u expect %d\n", v->name,
				got, v->expect_u8);
			return -1;
		}
		break;
	case FN_VHT_BW_BMP:
		got = rtw_get_tx_bw_bmp_of_vht_rate(&dvobj, (u8)v->rate, (u8)v->max_bw);
		if (got != (u8)v->expect_u8) {
			fprintf(stderr, "FAIL %s: vht bw bmp got %u expect %d\n", v->name,
				got, v->expect_u8);
			return -1;
		}
		break;
	case FN_ADAPTER_AGG_RATE_BMP: {
		u16 cck_by_bw[1] = {0};
		u32 ht_by_bw[2] = {0};
		u64 vht_by_bw[4] = {0};

		rtw_get_adapter_tx_rate_bmp(&adapter, cck_by_bw, ht_by_bw, vht_by_bw);
		if (cck_by_bw[0] != (u16)v->expect_cck_ofdm_20 ||
		    ht_by_bw[0] != v->expect_ht_20 || ht_by_bw[1] != v->expect_ht_40 ||
		    vht_by_bw[0] != v->expect_vht_20 ||
		    vht_by_bw[1] != v->expect_vht_40 ||
		    vht_by_bw[2] != v->expect_vht_80 ||
		    vht_by_bw[3] != v->expect_vht_160) {
			fprintf(stderr,
				"FAIL %s: aggregate bmp mismatch cck=0x%x ht=[0x%x,0x%x] vht=[0x%llx,0x%llx,0x%llx,0x%llx]\n",
				v->name, cck_by_bw[0], ht_by_bw[0], ht_by_bw[1],
				(unsigned long long)vht_by_bw[0],
				(unsigned long long)vht_by_bw[1],
				(unsigned long long)vht_by_bw[2],
				(unsigned long long)vht_by_bw[3]);
			return -1;
		}
		break;
	}
	case FN_QUERY_RA_SHORT_GI:
		sta.htpriv.sgi_20m = (u8)v->sgi_20m;
		sta.htpriv.sgi_40m = (u8)v->sgi_40m;
		sta.vhtpriv.vht_option = (u8)v->vht_option;
		sta.vhtpriv.sgi_80m = (u8)v->sgi_80m;
		got = query_ra_short_GI(&sta, (u8)v->bw);
		if (got != (u8)v->expect_u8) {
			fprintf(stderr, "FAIL %s: sgi got %u expect %d\n", v->name, got,
				v->expect_u8);
			return -1;
		}
		break;
	default:
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

static int vector_rust_ready(enum xmit_fn fn)
{
#ifdef RUST_XMIT_ORACLE
	switch (fn) {
	case FN_ADAPTER_AGG_RATE_BMP:
	case FN_QUERY_RA_SHORT_GI:
		return 1;
	default:
		return 1;
	}
#else
	return 1;
#endif
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	size_t executed = 0;
	size_t skipped = 0;
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
	for (i = 0; i < count; i++) {
		if (!vector_rust_ready(vectors[i].fn)) {
			printf("skip %s (c-only until W3-49 PR2)\n", vectors[i].name);
			skipped++;
			continue;
		}
		executed++;
		if (run_vector(&vectors[i]))
			failures++;
	}
	if (failures) {
		fprintf(stderr, "%zu vectors, %d failures\n", executed, failures);
		return 1;
	}
#ifdef RUST_XMIT_ORACLE
	if (skipped)
		printf("all %zu xmit vectors passed (%zu c-only skipped; oracle: rust/rtw_xmit.rs)\n",
		       executed, skipped);
	else
		printf("all %zu xmit vectors passed (oracle: rust/rtw_xmit.rs)\n", executed);
#else
	if (skipped)
		printf("all %zu xmit vectors passed (%zu skipped; oracle: core/rtw_xmit_rest.c)\n",
		       executed, skipped);
	else
		printf("all %zu xmit vectors passed (oracle: core/rtw_xmit_rest.c)\n", executed);
#endif
	return 0;
}
