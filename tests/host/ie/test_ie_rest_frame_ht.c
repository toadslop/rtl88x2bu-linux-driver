// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for frame header and HT MCS helpers (W3-32).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_FRAME 64
#define MAX_ESSID 32
#define MAX_HT_CAP 32
#define MAX_MCS_RATE 4

enum frame_ht_fn {
	FN_EMPTY_ESSID = 0,
	FN_GET_HDRLEN,
	FN_HT_MCS_RATE,
	FN_HT_CAP_RX_NSS,
	FN_HT_CAP_TX_NSS,
	FN_ACTION_FRAME_PARSE,
};

struct vector {
	char name[MAX_NAME];
	enum frame_ht_fn fn;
	char essid_hex[HOST_VECTOR_MAX_HEX_BUF];
	u8 essid[MAX_ESSID];
	size_t essid_len;
	u16 fc;
	u8 bw_40;
	u8 short_gi;
	u8 mcs_rate[MAX_MCS_RATE];
	u8 ht_cap[MAX_HT_CAP];
	size_t ht_cap_len;
	u8 frame[MAX_FRAME];
	size_t frame_len;
	int expect;
	u8 expect_category;
	u8 expect_action;
	int has_expect_category;
	int has_expect_action;
};

static int parse_fn(const char *obj, size_t obj_len, enum frame_ht_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "ieee80211_is_empty_essid") == 0)
		*out = FN_EMPTY_ESSID;
	else if (strcmp(fn, "ieee80211_get_hdrlen") == 0)
		*out = FN_GET_HDRLEN;
	else if (strcmp(fn, "rtw_ht_mcs_rate") == 0)
		*out = FN_HT_MCS_RATE;
	else if (strcmp(fn, "rtw_ht_cap_get_rx_nss") == 0)
		*out = FN_HT_CAP_RX_NSS;
	else if (strcmp(fn, "rtw_ht_cap_get_tx_nss") == 0)
		*out = FN_HT_CAP_TX_NSS;
	else if (strcmp(fn, "rtw_action_frame_parse") == 0)
		*out = FN_ACTION_FRAME_PARSE;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded_len = 0;

	memset(v, 0, sizeof(*v));

	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "expect", &v->expect))
		return -1;

	if (host_json_parse_string_in(obj, obj_len, "essid_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->essid, sizeof(v->essid), &decoded_len))
			return -1;
		v->essid_len = decoded_len;
	}

	host_json_parse_int_in(obj, obj_len, "fc", (int *)&v->fc);
	host_json_parse_int_in(obj, obj_len, "bw_40", (int *)&v->bw_40);
	host_json_parse_int_in(obj, obj_len, "short_gi", (int *)&v->short_gi);

	if (host_json_parse_string_in(obj, obj_len, "mcs_rate_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->mcs_rate, sizeof(v->mcs_rate), &decoded_len))
			return -1;
	}

	if (host_json_parse_string_in(obj, obj_len, "ht_cap_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->ht_cap, sizeof(v->ht_cap), &decoded_len))
			return -1;
		v->ht_cap_len = decoded_len;
	}

	if (host_json_parse_string_in(obj, obj_len, "frame_hex", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->frame, sizeof(v->frame), &decoded_len))
			return -1;
		v->frame_len = decoded_len;
	}

	if (host_json_parse_int_in(obj, obj_len, "expect_category",
				   (int *)&v->expect_category) == 0)
		v->has_expect_category = 1;
	if (host_json_parse_int_in(obj, obj_len, "expect_action",
				   (int *)&v->expect_action) == 0)
		v->has_expect_action = 1;

	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_EMPTY_ESSID: {
		int got = ieee80211_is_empty_essid((const char *)v->essid,
						   (int)v->essid_len);

		if (got != v->expect) {
			fprintf(stderr, "%s: ieee80211_is_empty_essid got %d expect %d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_GET_HDRLEN: {
		int got = ieee80211_get_hdrlen(v->fc);

		if (got != v->expect) {
			fprintf(stderr, "%s: ieee80211_get_hdrlen fc=0x%04x got %d expect %d\n",
				v->name, v->fc, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_HT_MCS_RATE: {
		u16 got = rtw_ht_mcs_rate(v->bw_40, v->short_gi, v->mcs_rate);

		if (got != (u16)v->expect) {
			fprintf(stderr, "%s: rtw_ht_mcs_rate got %u expect %d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_HT_CAP_RX_NSS: {
		u8 got = rtw_ht_cap_get_rx_nss(v->ht_cap);

		if (got != (u8)v->expect) {
			fprintf(stderr, "%s: rtw_ht_cap_get_rx_nss got %u expect %d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_HT_CAP_TX_NSS: {
		u8 got = rtw_ht_cap_get_tx_nss(v->ht_cap);

		if (got != (u8)v->expect) {
			fprintf(stderr, "%s: rtw_ht_cap_get_tx_nss got %u expect %d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_ACTION_FRAME_PARSE: {
		u8 category = 0;
		u8 action = 0;
		int got = rtw_action_frame_parse(v->frame, (u32)v->frame_len,
						 &category, &action);

		if (got != v->expect) {
			fprintf(stderr,
				"%s: rtw_action_frame_parse got %d expect %d\n",
				v->name, got, v->expect);
			return -1;
		}
		if (v->expect && v->has_expect_category &&
		    category != v->expect_category) {
			fprintf(stderr,
				"%s: category got %u expect %u\n",
				v->name, category, v->expect_category);
			return -1;
		}
		if (v->expect && v->has_expect_action && action != v->expect_action) {
			fprintf(stderr, "%s: action got %u expect %u\n",
				v->name, action, v->expect_action);
			return -1;
		}
		break;
	}
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0;
	size_t i;
	const char *path = (argc > 1) ? argv[1] : "ie_rest_frame_ht_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load vectors from %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL vector %zu/%zu: %s\n", i + 1, count,
				vectors[i].name);
			return 1;
		}
	}

#ifdef RUST_IEEE80211_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_ieee80211_rest.rs)\n", count);
#else
	printf("PASS %zu vectors (oracle: core/rtw_ieee80211_rest.c)\n", count);
#endif
	return 0;
}
