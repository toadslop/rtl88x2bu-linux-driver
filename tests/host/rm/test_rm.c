// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for RM util pure helpers (W3-33, W3-34). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rm_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 48
#define MAX_NAME 128
#define MAX_CH_SET 16

enum rm_fn {
	FN_DBM_RCPI,
	FN_PCT_RCPI,
	FN_WILDCARD_BSSID,
	FN_CH_SET,
	FN_OPER_CLASS,
	FN_DIALOG_TOKEN,
	FN_MEAS_TOKEN,
	FN_GEN_RMID,
};

struct vector {
	char name[MAX_NAME];
	enum rm_fn fn;
	s8 signal_dbm;
	u32 signal_pct;
	u8 bssid[6];
	u8 op_class;
	u8 ch_num;
	u8 ch;
	int expect;
	int expect_count;
	u8 expect_chs[MAX_CH_SET];
	u8 initial_dialog_token;
	u8 initial_meas_token;
	u8 diag_token;
	u8 role;
	u16 aid;
	int null_sta;
	int calls;
};

u8 rm_get_ch_set(struct rtw_ieee80211_channel *pch_set, u8 op_class, u8 ch_num);
u8 rm_get_oper_class_via_ch(u8 ch);
int is_wildcard_bssid(u8 *bssid);
u8 translate_dbm_to_rcpi(s8 SignalPower);
u8 translate_percentage_to_rcpi(u32 SignalStrengthIndex);
u8 rm_gen_dialog_token(_adapter *padapter);
u8 rm_gen_meas_token(_adapter *padapter);
u32 rm_gen_rmid(_adapter *padapter, struct rm_obj *prm, u8 role);

static int parse_fn(const char *obj, size_t len, enum rm_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "translate_dbm_to_rcpi"))
		*out = FN_DBM_RCPI;
	else if (!strcmp(fn, "translate_percentage_to_rcpi"))
		*out = FN_PCT_RCPI;
	else if (!strcmp(fn, "is_wildcard_bssid"))
		*out = FN_WILDCARD_BSSID;
	else if (!strcmp(fn, "rm_get_ch_set"))
		*out = FN_CH_SET;
	else if (!strcmp(fn, "rm_get_oper_class_via_ch"))
		*out = FN_OPER_CLASS;
	else if (!strcmp(fn, "rm_gen_dialog_token"))
		*out = FN_DIALOG_TOKEN;
	else if (!strcmp(fn, "rm_gen_meas_token"))
		*out = FN_MEAS_TOKEN;
	else if (!strcmp(fn, "rm_gen_rmid"))
		*out = FN_GEN_RMID;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	memset(v, 0, sizeof(*v));
	v->calls = 1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	if (v->fn != FN_CH_SET && v->fn != FN_GEN_RMID &&
	    host_json_parse_int_in(obj, len, "expect", &v->expect))
		return -1;
	host_json_parse_int_in(obj, len, "signal_dbm", (int *)&v->signal_dbm);
	host_json_parse_int_in(obj, len, "signal_pct", (int *)&v->signal_pct);
	host_json_parse_int_in(obj, len, "op_class", (int *)&v->op_class);
	host_json_parse_int_in(obj, len, "ch_num", (int *)&v->ch_num);
	host_json_parse_int_in(obj, len, "ch", (int *)&v->ch);
	host_json_parse_int_in(obj, len, "expect_count", &v->expect_count);
	host_json_parse_int_in(obj, len, "initial_dialog_token",
			       (int *)&v->initial_dialog_token);
	host_json_parse_int_in(obj, len, "initial_meas_token",
			       (int *)&v->initial_meas_token);
	host_json_parse_int_in(obj, len, "diag_token", (int *)&v->diag_token);
	host_json_parse_int_in(obj, len, "role", (int *)&v->role);
	host_json_parse_int_in(obj, len, "aid", (int *)&v->aid);
	host_json_parse_int_in(obj, len, "null_sta", &v->null_sta);
	host_json_parse_int_in(obj, len, "calls", &v->calls);
	if (host_json_parse_string_in(obj, len, "bssid_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->bssid, sizeof(v->bssid), &decoded) || decoded != 6)
			return -1;
	}
	if (host_json_parse_string_in(obj, len, "expect_chs_hex", hex, sizeof(hex)) == 0 &&
	    host_hex_decode(hex, v->expect_chs, sizeof(v->expect_chs), &decoded))
		return -1;
	if (v->fn == FN_GEN_RMID &&
	    host_json_parse_int_in(obj, len, "expect", &v->expect))
		return -1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	switch (v->fn) {
	case FN_DBM_RCPI: {
		u8 got = translate_dbm_to_rcpi(v->signal_dbm);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: dbm rcpi got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_PCT_RCPI: {
		u8 got = translate_percentage_to_rcpi(v->signal_pct);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: pct rcpi got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_WILDCARD_BSSID:
		if (is_wildcard_bssid((u8 *)v->bssid) != v->expect) {
			fprintf(stderr, "%s: wildcard bssid mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_OPER_CLASS: {
		u8 got = rm_get_oper_class_via_ch(v->ch);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: oper class got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CH_SET: {
		struct rtw_ieee80211_channel ch_set[MAX_CH_SET];
		int i;
		u8 got = rm_get_ch_set(ch_set, v->op_class, v->ch_num);

		if ((int)got != v->expect_count) {
			fprintf(stderr, "%s: ch_set count got %u want %d\n", v->name, got, v->expect_count);
			return -1;
		}
		for (i = 0; i < v->expect_count; i++) {
			if (ch_set[i].hw_value != v->expect_chs[i]) {
				fprintf(stderr, "%s: ch_set[%d] mismatch\n", v->name, i);
				return -1;
			}
		}
		break;
	}
	case FN_DIALOG_TOKEN: {
		_adapter adapter;
		int i;
		u8 got = 0;

		memset(&adapter, 0, sizeof(adapter));
		adapter.mlmeextpriv.mlmext_info.dialogToken = v->initial_dialog_token;
		for (i = 0; i < v->calls; i++)
			got = rm_gen_dialog_token(&adapter);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: dialog token got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_MEAS_TOKEN: {
		_adapter adapter;
		int i;
		u8 got = 0;

		memset(&adapter, 0, sizeof(adapter));
		adapter.rmpriv.meas_token = v->initial_meas_token;
		for (i = 0; i < v->calls; i++)
			got = rm_gen_meas_token(&adapter);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: meas token got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_GEN_RMID: {
		_adapter adapter;
		struct rm_obj rm;
		struct sta_info sta;
		u32 got;

		memset(&adapter, 0, sizeof(adapter));
		memset(&rm, 0, sizeof(rm));
		memset(&sta, 0, sizeof(sta));
		sta.cmn.aid = v->aid;
		rm.q.diag_token = v->diag_token;
		rm.psta = v->null_sta ? NULL : &sta;
		got = rm_gen_rmid(&adapter, &rm, v->role);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: rmid got %u want %d\n", v->name, got, v->expect);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "rm_vectors.json";

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
