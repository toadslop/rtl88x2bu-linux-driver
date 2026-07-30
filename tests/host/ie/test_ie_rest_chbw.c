// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for channel/bandwidth grouping helpers (W3-31).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_IE 256

enum chbw_fn {
	FN_IES_GET_CHBW = 0,
	FN_BSS_GET_CHBW,
	FN_IS_CHBW_GROUPED,
	FN_SYNC_CHBW,
};

struct vector {
	char name[MAX_NAME];
	enum chbw_fn fn;
	u8 ies[MAX_IE];
	size_t ies_len;
	u8 ht;
	u8 vht;
	u32 ds_config;
	u8 ch_a;
	u8 bw_a;
	u8 offset_a;
	u8 ch_b;
	u8 bw_b;
	u8 offset_b;
	u8 req_ch;
	u8 req_bw;
	u8 req_offset;
	u8 g_ch;
	u8 g_bw;
	u8 g_offset;
	u8 expect_ch;
	u8 expect_bw;
	u8 expect_offset;
	u8 expect_grouped;
	u8 expect_req_ch;
	u8 expect_req_bw;
	u8 expect_req_offset;
	u8 expect_g_bw;
	u8 expect_g_offset;
};

static int parse_fn(const char *obj, size_t obj_len, enum chbw_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_ies_get_chbw") == 0)
		*out = FN_IES_GET_CHBW;
	else if (strcmp(fn, "rtw_bss_get_chbw") == 0)
		*out = FN_BSS_GET_CHBW;
	else if (strcmp(fn, "rtw_is_chbw_grouped") == 0)
		*out = FN_IS_CHBW_GROUPED;
	else if (strcmp(fn, "rtw_sync_chbw") == 0)
		*out = FN_SYNC_CHBW;
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

	if (host_json_parse_string_in(obj, obj_len, "ies", hex, sizeof(hex))) {
		memset(hex, 0, sizeof(hex));
	} else if (hex[0] != '\0') {
		if (host_hex_decode(hex, v->ies, sizeof(v->ies), &decoded_len))
			return -1;
		v->ies_len = decoded_len;
	}

	host_json_parse_int_in(obj, obj_len, "ht", (int *)&v->ht);
	host_json_parse_int_in(obj, obj_len, "vht", (int *)&v->vht);
	host_json_parse_int_in(obj, obj_len, "ds_config", (int *)&v->ds_config);
	host_json_parse_int_in(obj, obj_len, "ch_a", (int *)&v->ch_a);
	host_json_parse_int_in(obj, obj_len, "bw_a", (int *)&v->bw_a);
	host_json_parse_int_in(obj, obj_len, "offset_a", (int *)&v->offset_a);
	host_json_parse_int_in(obj, obj_len, "ch_b", (int *)&v->ch_b);
	host_json_parse_int_in(obj, obj_len, "bw_b", (int *)&v->bw_b);
	host_json_parse_int_in(obj, obj_len, "offset_b", (int *)&v->offset_b);
	host_json_parse_int_in(obj, obj_len, "req_ch", (int *)&v->req_ch);
	host_json_parse_int_in(obj, obj_len, "req_bw", (int *)&v->req_bw);
	host_json_parse_int_in(obj, obj_len, "req_offset", (int *)&v->req_offset);
	host_json_parse_int_in(obj, obj_len, "g_ch", (int *)&v->g_ch);
	host_json_parse_int_in(obj, obj_len, "g_bw", (int *)&v->g_bw);
	host_json_parse_int_in(obj, obj_len, "g_offset", (int *)&v->g_offset);
	host_json_parse_int_in(obj, obj_len, "expect_ch", (int *)&v->expect_ch);
	host_json_parse_int_in(obj, obj_len, "expect_bw", (int *)&v->expect_bw);
	host_json_parse_int_in(obj, obj_len, "expect_offset", (int *)&v->expect_offset);
	host_json_parse_int_in(obj, obj_len, "expect_grouped", (int *)&v->expect_grouped);
	host_json_parse_int_in(obj, obj_len, "expect_req_ch", (int *)&v->expect_req_ch);
	host_json_parse_int_in(obj, obj_len, "expect_req_bw", (int *)&v->expect_req_bw);
	host_json_parse_int_in(obj, obj_len, "expect_req_offset",
			      (int *)&v->expect_req_offset);
	host_json_parse_int_in(obj, obj_len, "expect_g_bw", (int *)&v->expect_g_bw);
	host_json_parse_int_in(obj, obj_len, "expect_g_offset", (int *)&v->expect_g_offset);

	return 0;
}

static void setup_bss(WLAN_BSSID_EX *bss, struct vector *v)
{
	memset(bss, 0, sizeof(*bss));
	bss->Configuration.DSConfig = v->ds_config;
	bss->IELength = (u32)(sizeof(NDIS_802_11_FIXED_IEs) + v->ies_len);
	memcpy(bss->IEs + sizeof(NDIS_802_11_FIXED_IEs), v->ies, v->ies_len);
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_IES_GET_CHBW: {
		u8 ch = 0, bw = 0, offset = 0;

		rtw_ies_get_chbw(v->ies, (int)v->ies_len, &ch, &bw, &offset,
				 v->ht, v->vht);
		if (ch != v->expect_ch || bw != v->expect_bw ||
		    offset != v->expect_offset) {
			fprintf(stderr,
				"%s: ies_get chbw got=%u/%u/%u expect=%u/%u/%u\n",
				v->name, ch, bw, offset, v->expect_ch,
				v->expect_bw, v->expect_offset);
			return -1;
		}
		break;
	}
	case FN_BSS_GET_CHBW: {
		WLAN_BSSID_EX bss;
		u8 ch = 0, bw = 0, offset = 0;

		setup_bss(&bss, v);
		rtw_bss_get_chbw(&bss, &ch, &bw, &offset, v->ht, v->vht);
		if (ch != v->expect_ch || bw != v->expect_bw ||
		    offset != v->expect_offset) {
			fprintf(stderr,
				"%s: bss_get chbw got=%u/%u/%u expect=%u/%u/%u\n",
				v->name, ch, bw, offset, v->expect_ch,
				v->expect_bw, v->expect_offset);
			return -1;
		}
		break;
	}
	case FN_IS_CHBW_GROUPED: {
		bool grouped = rtw_is_chbw_grouped(v->ch_a, v->bw_a, v->offset_a,
						   v->ch_b, v->bw_b,
						   v->offset_b);

		if ((int)grouped != (int)v->expect_grouped) {
			fprintf(stderr, "%s: grouped got=%d expect=%u\n", v->name,
				(int)grouped, v->expect_grouped);
			return -1;
		}
		break;
	}
	case FN_SYNC_CHBW: {
		u8 req_ch = v->req_ch;
		u8 req_bw = v->req_bw;
		u8 req_offset = v->req_offset;
		u8 g_ch = v->g_ch;
		u8 g_bw = v->g_bw;
		u8 g_offset = v->g_offset;

		rtw_sync_chbw(&req_ch, &req_bw, &req_offset, &g_ch, &g_bw,
			      &g_offset);
		if (req_ch != v->expect_req_ch || req_bw != v->expect_req_bw ||
		    req_offset != v->expect_req_offset || g_bw != v->expect_g_bw ||
		    g_offset != v->expect_g_offset) {
			fprintf(stderr,
				"%s: sync got req=%u/%u/%u g=%u/%u/%u expect req=%u/%u/%u g=%u/%u\n",
				v->name, req_ch, req_bw, req_offset, g_ch, g_bw,
				g_offset, v->expect_req_ch, v->expect_req_bw,
				v->expect_req_offset, v->expect_g_bw,
				v->expect_g_offset);
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
	const char *path = "ie_rest_chbw_vectors.json";
	size_t i;
	int failures = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i]))
			failures++;
	}

	if (failures) {
		fprintf(stderr, "FAIL %d/%zu vectors\n", failures, count);
		return 1;
	}

#ifdef RUST_IEEE80211_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_ieee80211_rest.rs)\n", count);
#else
	printf("PASS %zu vectors (oracle: core/rtw_ieee80211_rest.c)\n", count);
#endif
	return 0;
}
