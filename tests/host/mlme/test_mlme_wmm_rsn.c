// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mlme_wmm_rsn_types.h"
#include "host_vector_json.h"

struct vector {
	char name[128], fn[64], expect_hex[512];
	u8 in[256], out[256], bssid[ETH_ALEN], pmkid[16];
	size_t in_len, out_off, expect_len;
	int expect_int, uapsd_max_sp_len, uapsd_tid, pmkid_used;
};

int rtw_restruct_wmm_ie(_adapter *a, u8 *in, u8 *out, unsigned in_len, unsigned out_off);
int rtw_cached_pmkid(_adapter *a, u8 *bssid);
int rtw_rsn_sync_pmkid(_adapter *a, u8 *ie, unsigned ie_len, int i_ent);

static int parse_vector_object(const char *obj, size_t len, void *v)
{
	struct vector *vec = v;
	char hex[512];
	size_t n = 0;
	int t = 0;

	memset(vec, 0, sizeof(*vec));
	if (host_json_parse_string_in(obj, len, "name", vec->name, sizeof(vec->name)) ||
	    host_json_parse_string_in(obj, len, "fn", vec->fn, sizeof(vec->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "uapsd_max_sp_len", &vec->uapsd_max_sp_len);
	host_json_parse_int_in(obj, len, "uapsd_tid", &vec->uapsd_tid);
	host_json_parse_int_in(obj, len, "pmkid_used", &vec->pmkid_used);
	host_json_parse_int_in(obj, len, "expect_len", &t);
	vec->expect_len = t;
	host_json_parse_int_in(obj, len, "expect_int", &vec->expect_int);
	if (!host_json_parse_string_in(obj, len, "in_ie_hex", hex, sizeof(hex))) {
		if (host_hex_decode(hex, vec->in, sizeof(vec->in), &n))
			return -1;
		host_json_parse_int_in(obj, len, "in_len", &t);
		vec->in_len = t ? (size_t)t : n;
	}
	if (!host_json_parse_string_in(obj, len, "bssid_hex", hex, sizeof(hex))) {
		if (host_hex_decode(hex, vec->bssid, sizeof(vec->bssid), &n) || n != ETH_ALEN)
			return -1;
	}
	host_json_parse_string_in(obj, len, "pmkid_hex", hex, sizeof(hex));
	host_hex_decode(hex, vec->pmkid, sizeof(vec->pmkid), &n);
	host_json_parse_int_in(obj, len, "initial_out_len", &t);
	vec->out_off = t;
	host_json_parse_string_in(obj, len, "expect_out_hex", vec->expect_hex,
				  sizeof(vec->expect_hex));
	return 0;
}

static void reset_adapter(_adapter *ad, const struct vector *vec)
{
	memset(ad, 0, sizeof(*ad));
	ad->mlmepriv.qospriv.uapsd_max_sp_len = (u8)vec->uapsd_max_sp_len;
	ad->mlmepriv.qospriv.uapsd_tid = (u16)vec->uapsd_tid;
	if (vec->pmkid_used) {
		ad->securitypriv.PMKIDList[0].bUsed = 1;
		memcpy(ad->securitypriv.PMKIDList[0].Bssid, vec->bssid, ETH_ALEN);
		memcpy(ad->securitypriv.PMKIDList[0].PMKID, vec->pmkid, 16);
	}
}

static int check_out(const u8 *out, size_t got, const char *expect_hex, size_t expect_len)
{
	u8 expect[256];
	size_t en = 0;

	if (got != expect_len || host_hex_decode(expect_hex, expect, sizeof(expect), &en) ||
	    en != got || memcmp(out, expect, en))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	_adapter ad = {0};

	if (argc != 2 || host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 16,
					   parse_vector_object, &n))
		return 1;
	for (size_t i = 0; i < n; i++) {
		struct vector *v = &vecs[i];
		unsigned got_u;

		reset_adapter(&ad, v);
		if (!strcmp(v->fn, "restruct_wmm_ie")) {
			memcpy(v->out, v->in, v->in_len);
			got_u = rtw_restruct_wmm_ie(&ad, v->in, v->out, (unsigned)v->in_len,
						    (unsigned)v->out_off);
			if (check_out(v->out, got_u, v->expect_hex, v->expect_len))
				goto fail;
		} else if (!strcmp(v->fn, "cached_pmkid")) {
			if (rtw_cached_pmkid(&ad, (u8 *)v->bssid) != v->expect_int)
				goto fail;
		} else if (!strcmp(v->fn, "rsn_sync_pmkid")) {
			memcpy(ad.scratch, v->in, v->in_len);
			got_u = rtw_rsn_sync_pmkid(&ad, ad.scratch, (unsigned)v->in_len, v->expect_int);
			if (check_out(ad.scratch, got_u, v->expect_hex, v->expect_len))
				goto fail;
		} else {
			goto fail;
		}
		printf("PASS %s\n", v->name);
		continue;
fail:
		return fprintf(stderr, "%s: FAIL\n", v->name), 1;
	}
	printf("PASS %zu vectors (oracle: %s) (%s)\n", n,
#ifdef RUST_MLME_WMM_RSN_ORACLE
	       "rust/rtw_mlme_wmm_rsn.rs",
#else
	       "core/rtw_mlme_rest.c",
#endif
	       argv[1]);
	return 0;
}
