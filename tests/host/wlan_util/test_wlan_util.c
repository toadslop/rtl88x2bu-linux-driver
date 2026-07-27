// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_wlan_util.c rate helpers (T5 / W3-08).
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"
#include "host_wlan_util_types.h"

#define MAX_VECTORS 64
#define MAX_NAME 128
#define MAX_RATES 32

enum wlan_fn {
	FN_IS_CCK = 0,
	FN_IS_OFDM,
	FN_IS_BASIC_CCK,
	FN_IS_BASIC_OFDM,
	FN_IS_BASIC_MIX,
	FN_CCK_INCLUDED,
	FN_CCK_ONLY,
};

struct vector {
	char name[MAX_NAME];
	enum wlan_fn fn;
	int rate;
	u8 rates[MAX_RATES];
	size_t rates_len;
	int expect;
};

#ifdef RUST_WLAN_UTIL_ORACLE
extern bool rtw_is_cck_rate(u8 rate);
extern bool rtw_is_ofdm_rate(u8 rate);
extern bool rtw_is_basic_rate_cck(u8 rate);
extern bool rtw_is_basic_rate_ofdm(u8 rate);
extern bool rtw_is_basic_rate_mix(u8 rate);
extern int cckrates_included(unsigned char *rate, int ratelen);
extern int cckratesonly_included(unsigned char *rate, int ratelen);
#else
bool rtw_is_cck_rate(u8 rate);
bool rtw_is_ofdm_rate(u8 rate);
bool rtw_is_basic_rate_cck(u8 rate);
bool rtw_is_basic_rate_ofdm(u8 rate);
bool rtw_is_basic_rate_mix(u8 rate);
int cckrates_included(unsigned char *rate, int ratelen);
int cckratesonly_included(unsigned char *rate, int ratelen);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum wlan_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_is_cck_rate") == 0)
		*out = FN_IS_CCK;
	else if (strcmp(fn, "rtw_is_ofdm_rate") == 0)
		*out = FN_IS_OFDM;
	else if (strcmp(fn, "rtw_is_basic_rate_cck") == 0)
		*out = FN_IS_BASIC_CCK;
	else if (strcmp(fn, "rtw_is_basic_rate_ofdm") == 0)
		*out = FN_IS_BASIC_OFDM;
	else if (strcmp(fn, "rtw_is_basic_rate_mix") == 0)
		*out = FN_IS_BASIC_MIX;
	else if (strcmp(fn, "cckrates_included") == 0)
		*out = FN_CCK_INCLUDED;
	else if (strcmp(fn, "cckratesonly_included") == 0)
		*out = FN_CCK_ONLY;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "rate", &v->rate);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	if (!host_json_parse_string_in(obj, obj_len, "rates", hex, sizeof(hex))) {
		if (host_hex_decode(hex, v->rates, sizeof(v->rates), &v->rates_len))
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	int got = 0;

	switch (v->fn) {
	case FN_IS_CCK:
		got = rtw_is_cck_rate((u8)v->rate) ? 1 : 0;
		break;
	case FN_IS_OFDM:
		got = rtw_is_ofdm_rate((u8)v->rate) ? 1 : 0;
		break;
	case FN_IS_BASIC_CCK:
		got = rtw_is_basic_rate_cck((u8)v->rate) ? 1 : 0;
		break;
	case FN_IS_BASIC_OFDM:
		got = rtw_is_basic_rate_ofdm((u8)v->rate) ? 1 : 0;
		break;
	case FN_IS_BASIC_MIX:
		got = rtw_is_basic_rate_mix((u8)v->rate) ? 1 : 0;
		break;
	case FN_CCK_INCLUDED:
		got = cckrates_included(v->rates, (int)v->rates_len);
		break;
	case FN_CCK_ONLY:
		got = cckratesonly_included(v->rates, (int)v->rates_len);
		break;
	default:
		return -1;
	}

	if (got != v->expect) {
		fprintf(stderr, "%s: mismatch got=%d expect=%d\n", v->name, got,
			v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "rate_class_vectors.json";
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
	printf("all %zu wlan_util rate vectors passed (oracle: core/rtw_wlan_util.c)\n",
	       nvec);
	return 0;
}
