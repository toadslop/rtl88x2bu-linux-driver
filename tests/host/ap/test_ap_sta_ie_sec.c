// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_sta_ie_sec_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int sec_wpa_psk;
	int sec_wpa2_group_cipher;
	int sec_wpa2_pairwise_cipher;
	int sec_akmp;
	char rsn_hex[128];
	int expect_status;
	int expect_wpa_psk;
	int expect_wpa2_group_cipher;
	int expect_wpa2_pairwise_cipher;
	int expect_flags;
};

static int parse_hex(const char *hex, u8 *out, size_t cap, size_t *len)
{
	size_t n = strlen(hex), i;

	if (n % 2)
		return -1;
	*len = n / 2;
	if (*len > cap)
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
	char hex[HOST_VECTOR_MAX_HEX_BUF] = {0};

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "sec_wpa_psk", &v->sec_wpa_psk);
	host_json_parse_int_in(obj, len, "sec_wpa2_group_cipher", &v->sec_wpa2_group_cipher);
	host_json_parse_int_in(obj, len, "sec_wpa2_pairwise_cipher", &v->sec_wpa2_pairwise_cipher);
	host_json_parse_int_in(obj, len, "sec_akmp", &v->sec_akmp);
	if (host_json_parse_string_in(obj, len, "rsn_ie", hex, sizeof(hex)) == 0 &&
	    hex[0]) {
		if (strlen(hex) >= sizeof(v->rsn_hex))
			return -1;
		memcpy(v->rsn_hex, hex, strlen(hex) + 1);
	}
	if (host_json_parse_int_in(obj, len, "expect_status", &v->expect_status))
		return -1;
	host_json_parse_int_in(obj, len, "expect_wpa_psk", &v->expect_wpa_psk);
	host_json_parse_int_in(obj, len, "expect_wpa2_group_cipher", &v->expect_wpa2_group_cipher);
	host_json_parse_int_in(obj, len, "expect_wpa2_pairwise_cipher", &v->expect_wpa2_pairwise_cipher);
	host_json_parse_int_in(obj, len, "expect_flags", &v->expect_flags);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "ap_sta_ie_sec_vectors.json";
	u8 rsn_buf[128];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count)) {
		fprintf(stderr, "FAIL: load vectors from %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		_adapter ad;
		struct sta_info sta;
		struct rtw_ieee802_11_elems elems;
		size_t rsn_len = 0;
		u16 status;

		memset(&ad, 0, sizeof(ad));
		memset(&sta, 0, sizeof(sta));
		memset(&elems, 0, sizeof(elems));

		ad.mlmepriv.cur_network.state = HOST_WIFI_AP_STATE;
		ad.securitypriv.wpa_psk = (u8)vectors[i].sec_wpa_psk;
		ad.securitypriv.wpa2_group_cipher = (unsigned int)vectors[i].sec_wpa2_group_cipher;
		ad.securitypriv.wpa2_pairwise_cipher = (unsigned int)vectors[i].sec_wpa2_pairwise_cipher;
		ad.securitypriv.akmp = (u32)vectors[i].sec_akmp;

		if (vectors[i].rsn_hex[0]) {
			if (parse_hex(vectors[i].rsn_hex, rsn_buf, sizeof(rsn_buf), &rsn_len))
				return 1;
			elems.rsn_ie = rsn_buf + 2;
			elems.rsn_ie_len = (u8)(rsn_len > 2 ? rsn_len - 2 : 0);
		}

		status = rtw_ap_parse_sta_security_ie(&ad, &sta, &elems);
		if (status != (u16)vectors[i].expect_status) {
			fprintf(stderr, "FAIL: %s status got %u expect %d\n",
				vectors[i].name, status, vectors[i].expect_status);
			return 1;
		}
		if (vectors[i].expect_wpa_psk &&
		    sta.wpa_psk != (u8)vectors[i].expect_wpa_psk) {
			fprintf(stderr, "FAIL: %s wpa_psk\n", vectors[i].name);
			return 1;
		}
		if (vectors[i].expect_wpa2_group_cipher &&
		    sta.wpa2_group_cipher != vectors[i].expect_wpa2_group_cipher) {
			fprintf(stderr, "FAIL: %s wpa2_group_cipher\n", vectors[i].name);
			return 1;
		}
		if (vectors[i].expect_wpa2_pairwise_cipher &&
		    sta.wpa2_pairwise_cipher != vectors[i].expect_wpa2_pairwise_cipher) {
			fprintf(stderr, "FAIL: %s wpa2_pairwise_cipher\n", vectors[i].name);
			return 1;
		}
		if (vectors[i].expect_flags &&
		    sta.flags != vectors[i].expect_flags) {
			fprintf(stderr, "FAIL: %s flags got %d expect %d\n",
				vectors[i].name, sta.flags, vectors[i].expect_flags);
			return 1;
		}
	}
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
