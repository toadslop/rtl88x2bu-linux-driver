// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_wlan_util.c ratetbl helpers (T5 / W3-09).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"
#include "host_wlan_util_types.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define NumRates 13

enum ratetbl_fn {
	FN_TBL_TO_WIFI = 0,
	FN_IS_BASIC,
	FN_TO_RATESET,
};

typedef struct {
	u8 oper_ch;
	struct {
		u8 basicrate[NumRates];
		u8 datarate[NumRates];
	} mlmeextpriv;
} host_wlan_adapter;

struct vector {
	char name[MAX_NAME];
	enum ratetbl_fn fn;
	int tbl_rate;
	int wifi_rate;
	int expect_rate;
	int expect;
	int expect_len;
	u8 expect_rates[NumRates];
	host_wlan_adapter adapter;
};

unsigned char host_ratetbl_val_2wifirate(unsigned char rate);
int host_is_basicrate(host_wlan_adapter *padapter, unsigned char rate);
unsigned int host_ratetbl2rateset(host_wlan_adapter *padapter,
				  unsigned char *rateset);

static int parse_fn(const char *obj, size_t obj_len, enum ratetbl_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "ratetbl_val_2wifirate") == 0)
		*out = FN_TBL_TO_WIFI;
	else if (strcmp(fn, "is_basicrate") == 0)
		*out = FN_IS_BASIC;
	else if (strcmp(fn, "ratetbl2rateset") == 0)
		*out = FN_TO_RATESET;
	else
		return -1;
	return 0;
}

static int parse_hex_tbl(const char *obj, size_t obj_len, const char *key,
			 u8 *out, size_t out_cap, int required)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t len = 0;

	memset(out, 0xff, out_cap);
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
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t expect_len = 0;

	memset(v, 0, sizeof(*v));
	memset(v->adapter.mlmeextpriv.basicrate, 0xff, NumRates);
	memset(v->adapter.mlmeextpriv.datarate, 0xff, NumRates);
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "tbl_rate", &v->tbl_rate);
	host_json_parse_int_in(obj, obj_len, "wifi_rate", &v->wifi_rate);
	host_json_parse_int_in(obj, obj_len, "expect_rate", &v->expect_rate);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	host_json_parse_int_in(obj, obj_len, "expect_len", &v->expect_len);
	{
		int oper_ch = 0;

		if (!host_json_parse_int_in(obj, obj_len, "oper_ch", &oper_ch))
			v->adapter.oper_ch = (u8)oper_ch;
	}
	if (parse_hex_tbl(obj, obj_len, "basicrate", v->adapter.mlmeextpriv.basicrate,
			  NumRates, v->fn != FN_TBL_TO_WIFI))
		return -1;
	if (parse_hex_tbl(obj, obj_len, "datarate", v->adapter.mlmeextpriv.datarate,
			  NumRates, v->fn == FN_TO_RATESET))
		return -1;
	if (!host_json_parse_string_in(obj, obj_len, "expect_rates", hex,
				       sizeof(hex))) {
		if (host_hex_decode(hex, v->expect_rates, sizeof(v->expect_rates),
				    &expect_len))
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_TBL_TO_WIFI: {
		unsigned char got = host_ratetbl_val_2wifirate((unsigned char)v->tbl_rate);

		if ((int)got != v->expect_rate) {
			fprintf(stderr, "%s: tbl_to_wifi got=%u expect=%d\n",
				v->name, got, v->expect_rate);
			return -1;
		}
		break;
	}
	case FN_IS_BASIC: {
		int got = host_is_basicrate(&v->adapter, (unsigned char)v->wifi_rate);

		if (got != v->expect) {
			fprintf(stderr, "%s: is_basic got=%d expect=%d\n", v->name,
				got, v->expect);
			return -1;
		}
		break;
	}
	case FN_TO_RATESET: {
		unsigned char rateset[NumRates];
		unsigned int len;

		memset(rateset, 0, sizeof(rateset));
		len = host_ratetbl2rateset(&v->adapter, rateset);
		if ((int)len != v->expect_len ||
		    memcmp(rateset, v->expect_rates, (size_t)v->expect_len) != 0) {
			fprintf(stderr, "%s: rateset mismatch len=%u\n", v->name, len);
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
	const char *path = "ratetbl_vectors.json";
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
	printf("all %zu ratetbl vectors passed (oracle: core/rtw_wlan_util.c)\n",
	       nvec);
	return 0;
}
