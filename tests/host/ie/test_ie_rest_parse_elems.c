// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for W3-76 element parse helpers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_parse_elems_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_IES 256

struct vector {
	char name[128];
	u8 ies[MAX_IES];
	size_t ies_len;
	int show_errors;
	int expect_result;
	int expect_ssid_len;
	int expect_ds_len;
	int expect_rsn_len;
	int expect_wpa_len;
	int expect_wps_len;
	int expect_wme_len;
	u8 expect_ssid_byte;
	u8 expect_ds_byte;
};

static int parse_vec(const char *obj, size_t len, void *v_)
{
	struct vector *v = v_;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "ies", hex, sizeof(hex)) ||
	    host_hex_decode(hex, v->ies, sizeof(v->ies), &v->ies_len))
		return -1;
	host_json_parse_int_in(obj, len, "show_errors", &v->show_errors);
	if (host_json_parse_int_in(obj, len, "expect_result", &v->expect_result))
		return -1;
	host_json_parse_int_in(obj, len, "expect_ssid_len", &v->expect_ssid_len);
	host_json_parse_int_in(obj, len, "expect_ds_len", &v->expect_ds_len);
	host_json_parse_int_in(obj, len, "expect_rsn_len", &v->expect_rsn_len);
	host_json_parse_int_in(obj, len, "expect_wpa_len", &v->expect_wpa_len);
	host_json_parse_int_in(obj, len, "expect_wps_len", &v->expect_wps_len);
	host_json_parse_int_in(obj, len, "expect_wme_len", &v->expect_wme_len);
	if (!host_json_parse_int_in(obj, len, "expect_ssid_byte", &tmp))
		v->expect_ssid_byte = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_ds_byte", &tmp))
		v->expect_ds_byte = (u8)tmp;
	return 0;
}

static int run_one(struct vector *v)
{
	struct rtw_ieee802_11_elems elems;
	ParseRes res;

	res = rtw_ieee802_11_parse_elems(v->ies, (unsigned int)v->ies_len, &elems,
					 v->show_errors);
	if (res != v->expect_result ||
	    (v->expect_ssid_len >= 0 && elems.ssid_len != (u8)v->expect_ssid_len) ||
	    (v->expect_ds_len >= 0 && elems.ds_params_len != (u8)v->expect_ds_len) ||
	    (v->expect_rsn_len >= 0 && elems.rsn_ie_len != (u8)v->expect_rsn_len) ||
	    (v->expect_wpa_len >= 0 && elems.wpa_ie_len != (u8)v->expect_wpa_len) ||
	    (v->expect_wps_len >= 0 && elems.wps_ie_len != (u8)v->expect_wps_len) ||
	    (v->expect_wme_len >= 0 && elems.wme_len != (u8)v->expect_wme_len) ||
	    (v->expect_ssid_len >= 0 && elems.ssid &&
	     elems.ssid[0] != v->expect_ssid_byte) ||
	    (v->expect_ds_len >= 0 && elems.ds_params &&
	     elems.ds_params[0] != v->expect_ds_byte))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vec, &n))
		return 2;
	for (i = 0; i < (int)n; i++)
		if (run_one(&vecs[i])) {
			fprintf(stderr, "%s: fail\n", vecs[i].name);
			fail++;
		}
#ifdef RUST_IEEE80211_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_ieee80211_rest.rs)\n", n);
#else
	printf("PASS %zu vectors (oracle: core/rtw_ieee80211.c)\n", n);
#endif
	return fail ? 1 : 0;
}
