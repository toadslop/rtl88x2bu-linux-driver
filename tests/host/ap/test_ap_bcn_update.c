// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_bcn_update_types.h"
#include "host_vector_json.h"

extern u8 host_bcn_update_last_erp_byte;

struct vector {
	char name[64];
	u8 erp_enable;
	int num_sta_non_erp, num_sta_no_short_preamble;
	u8 ies[256];
	size_t ies_len;
	int expect_erp_byte;
};

static int parse_hex(const char *hex, u8 *out, size_t cap, size_t *len)
{
	size_t n = strlen(hex), i;

	if (n % 2 || (*len = n / 2) > cap)
		return -1;
	for (i = 0; i < *len; i++) {
		unsigned v;
		if (sscanf(hex + i * 2, "%2x", &v) != 1)
			return -1;
		out[i] = (u8)v;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	int tmp;

	memset(v, 0, sizeof(*v));
	v->expect_erp_byte = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "ies_hex", hex, sizeof(hex)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "erp_enable", &tmp))
		v->erp_enable = (u8)tmp;
	host_json_parse_int_in(obj, len, "num_sta_non_erp", &v->num_sta_non_erp);
	host_json_parse_int_in(obj, len, "num_sta_no_short_preamble", &v->num_sta_no_short_preamble);
	host_json_parse_int_in(obj, len, "expect_erp_byte", &v->expect_erp_byte);
	return parse_hex(hex, v->ies, sizeof(v->ies), &v->ies_len);
}

static int run_vector(const struct vector *v)
{
	_adapter ad;
	WLAN_BSSID_EX *net = &ad.mlmeextpriv.mlmext_info.network;

	memset(&ad, 0, sizeof(ad));
	ad.mlmeextpriv.mlmext_info.ERP_enable = v->erp_enable;
	ad.mlmepriv.num_sta_non_erp = v->num_sta_non_erp;
	ad.mlmepriv.num_sta_no_short_preamble = v->num_sta_no_short_preamble;
	memcpy(net->IEs, v->ies, v->ies_len);
	net->IELength = (u32)v->ies_len;
	host_bcn_update_last_erp_byte = 0;
	update_bcn_erpinfo_ie(&ad);
	if (v->expect_erp_byte >= 0 &&
	    (int)host_bcn_update_last_erp_byte != v->expect_erp_byte) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
