// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_bcn_ie_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_FN 32
#define MAX_HEX 512

struct vector {
	char name[MAX_NAME];
	char fn[MAX_FN];
	char ies_in[MAX_HEX];
	int ie_len;
	int index;
	char data_hex[32];
	int data_len;
	char tim_bmp_hex[32];
	int tim_bmp_len;
	char expect_ies[MAX_HEX];
	int expect_ie_len;
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

static int hex_equal(const u8 *a, size_t alen, const char *hex)
{
	u8 buf[MAX_HEX];
	size_t blen;

	if (parse_hex(hex, buf, sizeof(buf), &blen))
		return 0;
	return alen == blen && memcmp(a, buf, alen) == 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (host_json_parse_string_in(obj, len, "ies_in", v->ies_in, sizeof(v->ies_in)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ie_len", &v->ie_len))
		return -1;
	host_json_parse_int_in(obj, len, "index", &v->index);
	if (host_json_parse_string_in(obj, len, "data", hex, sizeof(hex)) == 0 && hex[0]) {
		if (strlen(hex) >= sizeof(v->data_hex))
			return -1;
		memcpy(v->data_hex, hex, strlen(hex) + 1);
	}
	host_json_parse_int_in(obj, len, "data_len", &v->data_len);
	if (host_json_parse_string_in(obj, len, "tim_bmp", hex, sizeof(hex)) == 0 && hex[0]) {
		if (strlen(hex) >= sizeof(v->tim_bmp_hex))
			return -1;
		memcpy(v->tim_bmp_hex, hex, strlen(hex) + 1);
	}
	host_json_parse_int_in(obj, len, "tim_bmp_len", &v->tim_bmp_len);
	if (host_json_parse_string_in(obj, len, "expect_ies", v->expect_ies,
				      sizeof(v->expect_ies)))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_ie_len", &v->expect_ie_len))
		return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "ap_bcn_ie_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "FAIL: load vectors from %s\n", path);
		return 1;
	}

	for (i = 0; i < count; i++) {
		_adapter ad;
		WLAN_BSSID_EX net;
		u8 ies_buf[HOST_AP_BCN_IE_MAX_IE_SZ];
		u8 data_buf[32];
		size_t ies_len = 0, data_len = 0, tim_len = 0;

		memset(&ad, 0, sizeof(ad));
		memset(&net, 0, sizeof(net));
		if (parse_hex(vectors[i].ies_in, ies_buf, sizeof(ies_buf), &ies_len))
			return 1;
		memcpy(net.IEs, ies_buf, ies_len);
		net.IELength = (u32)vectors[i].ie_len;

		if (!strcmp(vectors[i].fn, "rtw_add_bcn_ie")) {
			if (vectors[i].data_hex[0] &&
			    parse_hex(vectors[i].data_hex, data_buf, sizeof(data_buf), &data_len))
				return 1;
			rtw_add_bcn_ie(&ad, &net, (u8)vectors[i].index, data_buf,
				       (u8)vectors[i].data_len);
		} else if (!strcmp(vectors[i].fn, "rtw_remove_bcn_ie")) {
			rtw_remove_bcn_ie(&ad, &net, (u8)vectors[i].index);
		} else if (!strcmp(vectors[i].fn, "update_BCNTIM")) {
			if (vectors[i].tim_bmp_hex[0] &&
			    parse_hex(vectors[i].tim_bmp_hex, ad.stapriv.tim_bitmap,
				      sizeof(ad.stapriv.tim_bitmap), &tim_len))
				return 1;
			ad.stapriv.aid_bmp_len = vectors[i].tim_bmp_len ?
						 (u8)vectors[i].tim_bmp_len : (u8)tim_len;
			ad.mlmeextpriv.mlmext_info.network = net;
			update_BCNTIM(&ad);
			net = ad.mlmeextpriv.mlmext_info.network;
		} else {
			fprintf(stderr, "FAIL: unknown fn %s\n", vectors[i].fn);
			return 1;
		}

		if ((int)net.IELength != vectors[i].expect_ie_len) {
			fprintf(stderr, "FAIL: %s IELength got %u expect %d\n",
				vectors[i].name, net.IELength, vectors[i].expect_ie_len);
			return 1;
		}
		if (!hex_equal(net.IEs, net.IELength, vectors[i].expect_ies)) {
			fprintf(stderr, "FAIL: %s IE bytes mismatch\n", vectors[i].name);
			return 1;
		}
	}
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
