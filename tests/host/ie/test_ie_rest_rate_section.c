// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-41 rate-section / ch-offset helpers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_rate_section_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 24
#define MAX_NAME 128
#define MAX_RATES 32

enum fn_id {
	FN_RS, FN_CCK_SZ, FN_CCK_IN, FN_CCK_ONLY, FN_RS_LEN, FN_SEC_HAL, FN_HAL_SEC
};

struct vector {
	char name[MAX_NAME];
	enum fn_id fn;
	int arg, arg2;
	u8 rates[MAX_RATES];
	size_t rates_len;
	int expect;
};

static int parse_fn(const char *obj, size_t len, enum fn_id *fn)
{
	char s[64];
	if (host_json_parse_string_in(obj, len, "fn", s, sizeof(s)))
		return -1;
	if (!strcmp(s, "mgn_rate_to_rs"))
		*fn = FN_RS;
	else if (!strcmp(s, "rtw_get_cckrate_size"))
		*fn = FN_CCK_SZ;
	else if (!strcmp(s, "rtw_is_cckrates_included"))
		*fn = FN_CCK_IN;
	else if (!strcmp(s, "rtw_is_cckratesonly_included"))
		*fn = FN_CCK_ONLY;
	else if (!strcmp(s, "rtw_get_rateset_len"))
		*fn = FN_RS_LEN;
	else if (!strcmp(s, "secondary_ch_offset_to_hal_ch_offset"))
		*fn = FN_SEC_HAL;
	else if (!strcmp(s, "hal_ch_offset_to_secondary_ch_offset"))
		*fn = FN_HAL_SEC;
	else
		return -1;
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *v_)
{
	struct vector *v = v_;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, len, "rate", &v->arg);
	host_json_parse_int_in(obj, len, "rate_length", &v->arg2);
	host_json_parse_int_in(obj, len, "ch_offset", &v->arg);
	host_json_parse_int_in(obj, len, "expect_u32", &v->expect);
	host_json_parse_int_in(obj, len, "expect_u8", &v->expect);
	if (!host_json_parse_string_in(obj, len, "rates", hex, sizeof(hex)) && hex[0])
		return host_hex_decode(hex, v->rates, sizeof(v->rates), &v->rates_len);
	return 0;
}

static int run_one(struct vector *v)
{
	int got;
	switch (v->fn) {
	case FN_RS:
		got = mgn_rate_to_rs((enum MGN_RATE)v->arg);
		break;
	case FN_CCK_SZ:
		got = rtw_get_cckrate_size(v->rates, (unsigned int)v->arg2);
		break;
	case FN_CCK_IN:
		got = rtw_is_cckrates_included(v->rates);
		break;
	case FN_CCK_ONLY:
		got = rtw_is_cckratesonly_included(v->rates);
		break;
	case FN_RS_LEN:
		got = rtw_get_rateset_len(v->rates);
		break;
	case FN_SEC_HAL:
		got = secondary_ch_offset_to_hal_ch_offset((u8)v->arg);
		break;
	case FN_HAL_SEC:
		got = hal_ch_offset_to_secondary_ch_offset((u8)v->arg);
		break;
	default:
		return -1;
	}
	if (got != v->expect) {
		fprintf(stderr, "FAIL %s: got %d expect %d\n", v->name, got, v->expect);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0, i;
	int fail = 0;
	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (i = 0; i < n; i++)
		fail += run_one(&vecs[i]) != 0;
	if (fail)
		return 1;
#ifdef RUST_IEEE80211_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_ieee80211_rest.rs)\n", n);
#else
	printf("PASS %zu vectors (oracle: core/rtw_ieee80211_rest.c)\n", n);
#endif
	return 0;
}
