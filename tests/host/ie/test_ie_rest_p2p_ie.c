// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-43 P2P IE merge/delete helpers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_IE 128
#define FIXED_IE_LEN 12

enum fn_id {
	FN_MERGED_LEN,
	FN_MERGE,
	FN_SET_ATTR,
	FN_DEL_IE,
	FN_DEL_ATTR,
	FN_BSS_DEL_IE,
	FN_BSS_DEL_ATTR,
};

struct vector {
	char name[128];
	enum fn_id fn;
	u8 ie[MAX_IE];
	size_t ie_len;
	u8 fixed[FIXED_IE_LEN];
	u8 expect[MAX_IE];
	size_t expect_len;
	u8 merge_out[MAX_IE];
	size_t merge_out_len;
	u8 attr_data[16];
	u8 attr_id;
	u16 attr_len;
	u32 expect_u32;
	int expect_i32;
};

static int parse_fn(const char *s, enum fn_id *fn)
{
	if (!strcmp(s, "rtw_get_p2p_merged_ies_len")) *fn = FN_MERGED_LEN;
	else if (!strcmp(s, "rtw_p2p_merge_ies")) *fn = FN_MERGE;
	else if (!strcmp(s, "rtw_set_p2p_attr_content")) *fn = FN_SET_ATTR;
	else if (!strcmp(s, "rtw_del_p2p_ie")) *fn = FN_DEL_IE;
	else if (!strcmp(s, "rtw_del_p2p_attr")) *fn = FN_DEL_ATTR;
	else if (!strcmp(s, "rtw_bss_ex_del_p2p_ie")) *fn = FN_BSS_DEL_IE;
	else if (!strcmp(s, "rtw_bss_ex_del_p2p_attr")) *fn = FN_BSS_DEL_ATTR;
	else return -1;
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *v_)
{
	struct vector *v = v_;
	char hex[HOST_VECTOR_MAX_HEX_BUF], s[64];
	size_t dummy;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", s, sizeof(s)) || parse_fn(s, &v->fn))
		return -1;
	if (!host_json_parse_int_in(obj, len, "attr_id", &tmp)) v->attr_id = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "attr_len", &tmp)) v->attr_len = (u16)tmp;
	host_json_parse_int_in(obj, len, "expect_u32", (int *)&v->expect_u32);
	if (host_json_parse_int_in(obj, len, "expect_i32", &v->expect_i32) != 0)
		v->expect_i32 = -1;
	if (!host_json_parse_string_in(obj, len, "fixed_ies", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->fixed, sizeof(v->fixed), &dummy);
	if (!host_json_parse_string_in(obj, len, "ie", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->ie, sizeof(v->ie), &v->ie_len);
	if (!host_json_parse_string_in(obj, len, "attr_data", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->attr_data, sizeof(v->attr_data), &dummy);
	if (!host_json_parse_string_in(obj, len, "expect_ie", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->expect, sizeof(v->expect), &v->expect_len);
	if (!host_json_parse_string_in(obj, len, "expect_out", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->expect, sizeof(v->expect), &v->expect_len);
	if (!host_json_parse_string_in(obj, len, "merge_out", hex, sizeof(hex)) && hex[0])
		host_hex_decode(hex, v->merge_out, sizeof(v->merge_out), &v->merge_out_len);
	return 0;
}

static void init_bss(WLAN_BSSID_EX *bss, const u8 *fixed, const u8 *tlv, size_t tlv_len)
{
	memset(bss, 0, sizeof(*bss));
	memcpy(bss->IEs, fixed, FIXED_IE_LEN);
	if (tlv_len)
		memcpy(bss->IEs + FIXED_IE_LEN, tlv, tlv_len);
	bss->IELength = FIXED_IE_LEN + (u32)tlv_len;
}

static int run_one(struct vector *v)
{
	u8 buf[MAX_IE];
	WLAN_BSSID_EX bss;

	switch (v->fn) {
	case FN_MERGED_LEN:
		if (rtw_get_p2p_merged_ies_len(v->ie, (u32)v->ie_len) != v->expect_u32)
			return -1;
		return 0;
	case FN_MERGE: {
		u8 out[MAX_IE];

		if (rtw_p2p_merge_ies(v->ie, (u32)v->ie_len, out) != v->expect_i32 ||
		    (v->merge_out_len &&
		     memcmp(out, v->merge_out, v->merge_out_len)))
			return -1;
		return 0;
	}
	case FN_SET_ATTR: {
		u8 out[MAX_IE];
		if (rtw_set_p2p_attr_content(out, v->attr_id, v->attr_len, v->attr_data) != v->expect_u32 ||
		    memcmp(out, v->expect, v->expect_len))
			return -1;
		return 0;
	}
	case FN_DEL_IE:
		memcpy(buf, v->ie, v->ie_len);
		if (rtw_del_p2p_ie(buf, (unsigned int)v->ie_len, NULL) != v->expect_u32 ||
		    memcmp(buf, v->expect, v->expect_len))
			return -1;
		return 0;
	case FN_DEL_ATTR:
		memcpy(buf, v->ie, v->ie_len);
		if (rtw_del_p2p_attr(buf, (unsigned int)v->ie_len, v->attr_id) != v->expect_u32 ||
		    memcmp(buf, v->expect, v->expect_len))
			return -1;
		return 0;
	case FN_BSS_DEL_IE:
		init_bss(&bss, v->fixed, v->ie, v->ie_len);
		rtw_bss_ex_del_p2p_ie(&bss);
		if (bss.IELength != v->expect_u32 ||
		    memcmp(BSS_EX_TLV_IES(&bss), v->expect, v->expect_len))
			return -1;
		return 0;
	case FN_BSS_DEL_ATTR:
		init_bss(&bss, v->fixed, v->ie, v->ie_len);
		rtw_bss_ex_del_p2p_attr(&bss, v->attr_id);
		if (bss.IELength != v->expect_u32 ||
		    memcmp(BSS_EX_TLV_IES(&bss), v->expect, v->expect_len))
			return -1;
		return 0;
	default:
		return -1;
	}
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int i, fail = 0;

	if (argc != 2 || host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (i = 0; i < (int)n; i++)
		if (run_one(&vecs[i])) {
			fprintf(stderr, "%s: fail\n", vecs[i].name);
			fail++;
		}
	printf("p2p_ie: %zu vectors, %d failures\n", n, fail);
	return fail ? 1 : 0;
}
