// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-42 HT MCS bitmap and AMSDU mode helpers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

u8 rtw_ht_mcsset_to_nss(u8 *supp_mcs_set);
u32 rtw_ht_mcs_set_to_bitmap(u8 *mcs_set, u8 nss);
void rtw_set_spp_amsdu_mode(u8 mode, u8 *rsn_ie, int rsn_ie_len);
u8 rtw_check_amsdu_disable(u8 mode, u8 spp_opt);

#define MAX_VECTORS 24
#define MAX_NAME 128
#define MAX_MCS 4
#define MAX_IE 64

enum fn_id {
	FN_HT_MCSSet_TO_NSS,
	FN_HT_MCS_SET_TO_BITMAP,
	FN_CHECK_AMSDU_DISABLE,
	FN_SET_SPP_AMSDU_MODE,
};

struct vector {
	char name[MAX_NAME];
	enum fn_id fn;
	u8 mcs_set[MAX_MCS];
	size_t mcs_len;
	u8 nss;
	u8 mode;
	u8 spp_opt;
	u8 ie[MAX_IE];
	size_t ie_len;
	int expect_u8;
	u32 expect_u32;
	int has_expect_u32;
};

static int parse_fn(const char *obj, size_t len, enum fn_id *fn)
{
	char s[64];

	if (host_json_parse_string_in(obj, len, "fn", s, sizeof(s)))
		return -1;
	if (!strcmp(s, "rtw_ht_mcsset_to_nss"))
		*fn = FN_HT_MCSSet_TO_NSS;
	else if (!strcmp(s, "rtw_ht_mcs_set_to_bitmap"))
		*fn = FN_HT_MCS_SET_TO_BITMAP;
	else if (!strcmp(s, "rtw_check_amsdu_disable"))
		*fn = FN_CHECK_AMSDU_DISABLE;
	else if (!strcmp(s, "rtw_set_spp_amsdu_mode"))
		*fn = FN_SET_SPP_AMSDU_MODE;
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
	{
		int tmp;
		if (!host_json_parse_int_in(obj, len, "nss", &tmp))
			v->nss = (u8)tmp;
		if (!host_json_parse_int_in(obj, len, "mode", &tmp))
			v->mode = (u8)tmp;
		if (!host_json_parse_int_in(obj, len, "spp_opt", &tmp))
			v->spp_opt = (u8)tmp;
		host_json_parse_int_in(obj, len, "expect_u8", &v->expect_u8);
	}
	if (!host_json_parse_int_in(obj, len, "expect_u32", (int *)&v->expect_u32))
		v->has_expect_u32 = 1;
	if (!host_json_parse_string_in(obj, len, "mcs_set", hex, sizeof(hex)) && hex[0])
		return host_hex_decode(hex, v->mcs_set, sizeof(v->mcs_set), &v->mcs_len);
	if (!host_json_parse_string_in(obj, len, "ie", hex, sizeof(hex)) && hex[0])
		return host_hex_decode(hex, v->ie, sizeof(v->ie), &v->ie_len);
	return 0;
}

static const u8 *rsn_cap_ptr(const u8 *ie, size_t ie_len)
{
	struct rsne_info info;

	if (rtw_rsne_info_parse(ie, (unsigned int)ie_len, &info) != _SUCCESS)
		return NULL;
	return info.cap;
}

static int run_one(struct vector *v)
{
	switch (v->fn) {
	case FN_HT_MCSSet_TO_NSS: {
		int got = rtw_ht_mcsset_to_nss(v->mcs_set);
		if (got != v->expect_u8) {
			fprintf(stderr, "%s: nss got %d expect %d\n", v->name, got, v->expect_u8);
			return -1;
		}
		return 0;
	}
	case FN_HT_MCS_SET_TO_BITMAP: {
		u32 got = rtw_ht_mcs_set_to_bitmap(v->mcs_set, v->nss);
		if (got != v->expect_u32) {
			fprintf(stderr, "%s: bitmap got %u expect %u\n", v->name, got, v->expect_u32);
			return -1;
		}
		return 0;
	}
	case FN_CHECK_AMSDU_DISABLE: {
		int got = rtw_check_amsdu_disable(v->mode, v->spp_opt);
		if (got != v->expect_u8) {
			fprintf(stderr, "%s: disable got %d expect %d\n", v->name, got, v->expect_u8);
			return -1;
		}
		return 0;
	}
	case FN_SET_SPP_AMSDU_MODE: {
		const u8 *cap_before = rsn_cap_ptr(v->ie, v->ie_len);
		u8 opt_before;

		if (!cap_before)
			return -1;
		opt_before = GET_RSN_CAP_SPP_OPT(cap_before);
		rtw_set_spp_amsdu_mode(v->mode, v->ie, (int)v->ie_len);
		if (GET_RSN_CAP_SPP_OPT(cap_before) != (u8)v->expect_u8) {
			fprintf(stderr, "%s: spp opt got %u expect %u (before %u)\n",
				v->name, GET_RSN_CAP_SPP_OPT(cap_before), v->expect_u8, opt_before);
			return -1;
		}
		return 0;
	}
	default:
		return -1;
	}
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int i, failed = 0;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (i = 0; i < (int)n; i++)
		if (run_one(&vecs[i]))
			failed++;
#ifdef RUST_IEEE80211_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_ieee80211_rest.rs)\n", n);
#else
	printf("PASS %zu vectors (oracle: core/rtw_ieee80211_rest.c)\n", n);
#endif
	return failed ? 1 : 0;
}
