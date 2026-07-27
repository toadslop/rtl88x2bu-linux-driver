// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_wlan_util.c tail helpers (W3-09b).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_chplan_types.h"
#include "host_vector_json.h"
#include "host_wlan_util_types.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define NumRates 13

enum tail_fn {
	FN_JUDGE_NET = 0,
	FN_MCS_MASK,
	FN_GET_RATESET,
};

typedef struct {
	u8 oper_ch;
	struct {
		u8 basicrate[NumRates];
		u8 datarate[NumRates];
	} mlmeextpriv;
} host_wlan_adapter;

typedef struct {
	char _pad0[0x844];
	u8 cur_channel;
	char _pad1[0xc58 - 0x844 - 1];
	u8 ht_enable;
	char _pad2[0xc66 - 0xc58 - 1];
	u8 vht_enable;
} host_network_adapter;

struct vector {
	char name[MAX_NAME];
	enum tail_fn fn;
	int cur_channel;
	int ht_enable;
	int vht_enable;
	int oper_ch;
	u8 rates[NumRates];
	size_t rates_len;
	u32 mask;
	u8 mcs_set[4];
	u8 expect_mcs[4];
	int expect;
	int expect_len;
	u8 expect_rates[NumRates];
	host_wlan_adapter adapter;
};

u8 judge_network_type(host_network_adapter *padapter, unsigned char *rate,
		      int ratelen);
void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask);
void get_rate_set(host_wlan_adapter *padapter, unsigned char *pbssrate,
		  int *bssrate_len);

static int parse_fn(const char *obj, size_t obj_len, enum tail_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "judge_network_type") == 0)
		*out = FN_JUDGE_NET;
	else if (strcmp(fn, "set_mcs_rate_by_mask") == 0)
		*out = FN_MCS_MASK;
	else if (strcmp(fn, "get_rate_set") == 0)
		*out = FN_GET_RATESET;
	else
		return -1;
	return 0;
}

static int parse_hex_tbl(const char *obj, size_t obj_len, const char *key,
			 u8 *out, size_t out_cap, int required)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t len = 0;

	memset(out, 0, out_cap);
	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex))) {
		if (required)
			return -1;
		return 0;
	}
	if (host_hex_decode(hex, out, out_cap, &len))
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	memset(v->adapter.mlmeextpriv.basicrate, 0xff, NumRates);
	memset(v->adapter.mlmeextpriv.datarate, 0xff, NumRates);
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "cur_channel", &v->cur_channel);
	host_json_parse_int_in(obj, obj_len, "ht_enable", &v->ht_enable);
	host_json_parse_int_in(obj, obj_len, "vht_enable", &v->vht_enable);
	host_json_parse_int_in(obj, obj_len, "oper_ch", &v->oper_ch);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	host_json_parse_int_in(obj, obj_len, "expect_len", &v->expect_len);
	{
		int mask = 0;

		if (!host_json_parse_int_in(obj, obj_len, "mask", &mask))
			v->mask = (u32)mask;
	}
	{
		char hex[HOST_VECTOR_MAX_HEX_BUF];
		size_t rates_len = 0;

		if (!host_json_parse_string_in(obj, obj_len, "rates", hex, sizeof(hex)))
			host_hex_decode(hex, v->rates, sizeof(v->rates), &rates_len);
		v->rates_len = rates_len;
	}
	parse_hex_tbl(obj, obj_len, "mcs_set", v->mcs_set, sizeof(v->mcs_set), 0);
	parse_hex_tbl(obj, obj_len, "expect_mcs", v->expect_mcs, sizeof(v->expect_mcs),
		      0);
	if (parse_hex_tbl(obj, obj_len, "basicrate", v->adapter.mlmeextpriv.basicrate,
			  NumRates, v->fn == FN_GET_RATESET))
		return -1;
	if (parse_hex_tbl(obj, obj_len, "datarate", v->adapter.mlmeextpriv.datarate,
			  NumRates, v->fn == FN_GET_RATESET))
		return -1;
	parse_hex_tbl(obj, obj_len, "expect_rates", v->expect_rates,
		      sizeof(v->expect_rates), 0);
	if (v->fn == FN_GET_RATESET)
		v->adapter.oper_ch = (u8)v->oper_ch;
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_JUDGE_NET: {
		host_network_adapter net;
		u8 got;

		memset(&net, 0, sizeof(net));
		net.cur_channel = (u8)v->cur_channel;
		net.ht_enable = (u8)v->ht_enable;
		net.vht_enable = (u8)v->vht_enable;
		got = judge_network_type(&net, v->rates, (int)v->rates_len);
		if ((int)got != v->expect) {
			fprintf(stderr, "%s: net_type got=%u expect=%d\n", v->name,
				got, v->expect);
			return -1;
		}
		break;
	}
	case FN_MCS_MASK: {
		u8 mcs[4];

		memcpy(mcs, v->mcs_set, sizeof(mcs));
		set_mcs_rate_by_mask(mcs, v->mask);
		if (memcmp(mcs, v->expect_mcs, sizeof(mcs)) != 0) {
			fprintf(stderr, "%s: mcs_set mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_GET_RATESET: {
		unsigned char rateset[NumRates];
		int len = 0;

		memset(rateset, 0, sizeof(rateset));
		get_rate_set(&v->adapter, rateset, &len);
		if (len != v->expect_len ||
		    memcmp(rateset, v->expect_rates, (size_t)len) != 0) {
			fprintf(stderr, "%s: get_rate_set mismatch len=%d\n", v->name,
				len);
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
	const char *path = "wlan_tail_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu wlan_util tail vectors passed (oracle: core/rtw_wlan_util.c)\n",
	       nvec);
	return 0;
}
