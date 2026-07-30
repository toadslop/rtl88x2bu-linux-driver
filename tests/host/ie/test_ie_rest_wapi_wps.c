// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WAPI/WPS/sec-IE getter helpers (W3-29).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_IE 256
#define MAX_OUT_IE 64

enum wapi_wps_fn {
	FN_GET_WAPI_IE = 0,
	FN_GET_SEC_IE,
	FN_IS_WPS_IE,
};

struct vector {
	char name[MAX_NAME];
	enum wapi_wps_fn fn;
	u8 ie[MAX_IE];
	size_t ie_len;
	int expect_ret;
	u16 expect_wapi_len;
	u8 expect_wapi_ie[MAX_OUT_IE];
	size_t expect_wapi_ie_len;
	u16 expect_wpa_len;
	u16 expect_rsn_len;
	u8 expect_wpa_ie[MAX_OUT_IE];
	u8 expect_rsn_ie[MAX_OUT_IE];
	size_t expect_wpa_ie_len;
	size_t expect_rsn_ie_len;
	u8 expect_match;
	unsigned int expect_wps_ielen;
	int no_wapi_buf;
	int null_ie_ptr;
};

static int parse_fn(const char *obj, size_t obj_len, enum wapi_wps_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_get_wapi_ie") == 0)
		*out = FN_GET_WAPI_IE;
	else if (strcmp(fn, "rtw_get_sec_ie") == 0)
		*out = FN_GET_SEC_IE;
	else if (strcmp(fn, "rtw_is_wps_ie") == 0)
		*out = FN_IS_WPS_IE;
	else
		return -1;
	return 0;
}

static int parse_optional_hex_ie(const char *obj, size_t obj_len, const char *key,
				 u8 *buf, size_t buf_sz, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded_len = 0;

	*out_len = 0;
	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return 0;
	if (hex[0] == '\0')
		return 0;
	if (host_hex_decode(hex, buf, buf_sz, &decoded_len))
		return -1;
	*out_len = decoded_len;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded_len = 0;
	int json_ie_len = -1;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (!host_json_parse_int_in(obj, obj_len, "ie_len", &json_ie_len))
		v->ie_len = (size_t)json_ie_len;
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "expect_wapi_len", (int *)&v->expect_wapi_len);
	host_json_parse_int_in(obj, obj_len, "expect_wpa_len", (int *)&v->expect_wpa_len);
	host_json_parse_int_in(obj, obj_len, "expect_rsn_len", (int *)&v->expect_rsn_len);
	host_json_parse_int_in(obj, obj_len, "expect_match", (int *)&v->expect_match);
	host_json_parse_int_in(obj, obj_len, "expect_wps_ielen", (int *)&v->expect_wps_ielen);
	host_json_parse_int_in(obj, obj_len, "no_wapi_buf", &v->no_wapi_buf);
	host_json_parse_int_in(obj, obj_len, "null_ie_ptr", &v->null_ie_ptr);
	if (host_json_parse_string_in(obj, obj_len, "ie", hex, sizeof(hex)))
		return -1;
	if (hex[0] != '\0') {
		if (host_hex_decode(hex, v->ie, sizeof(v->ie), &decoded_len))
			return -1;
		if (json_ie_len < 0)
			v->ie_len = decoded_len;
	}
	if (parse_optional_hex_ie(obj, obj_len, "expect_wapi_ie", v->expect_wapi_ie,
				  sizeof(v->expect_wapi_ie), &v->expect_wapi_ie_len))
		return -1;
	if (parse_optional_hex_ie(obj, obj_len, "expect_wpa_ie", v->expect_wpa_ie,
				  sizeof(v->expect_wpa_ie), &v->expect_wpa_ie_len))
		return -1;
	if (parse_optional_hex_ie(obj, obj_len, "expect_rsn_ie", v->expect_rsn_ie,
				  sizeof(v->expect_rsn_ie), &v->expect_rsn_ie_len))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_GET_WAPI_IE: {
		u8 wapi_ie[MAX_OUT_IE];
		u16 wapi_len = 0;
		u8 *wapi_ptr = v->no_wapi_buf ? NULL : wapi_ie;
		int ret = rtw_get_wapi_ie(v->ie, (unsigned int)v->ie_len, wapi_ptr,
					  &wapi_len);

		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (wapi_len != v->expect_wapi_len) {
			fprintf(stderr, "%s: wapi_len got %u expect %u\n", v->name,
				wapi_len, v->expect_wapi_len);
			return -1;
		}
		if (!v->no_wapi_buf && v->expect_wapi_ie_len &&
		    memcmp(wapi_ie, v->expect_wapi_ie, v->expect_wapi_ie_len)) {
			fprintf(stderr, "%s: wapi_ie mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_GET_SEC_IE: {
		u8 rsn_ie[MAX_OUT_IE];
		u8 wpa_ie[MAX_OUT_IE];
		u16 rsn_len = 0;
		u16 wpa_len = 0;
		int ret = rtw_get_sec_ie(v->ie, (unsigned int)v->ie_len, rsn_ie, &rsn_len,
					 wpa_ie, &wpa_len);

		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: ret got %d expect %d\n", v->name, ret,
				v->expect_ret);
			return -1;
		}
		if (wpa_len != v->expect_wpa_len) {
			fprintf(stderr, "%s: wpa_len got %u expect %u\n", v->name, wpa_len,
				v->expect_wpa_len);
			return -1;
		}
		if (rsn_len != v->expect_rsn_len) {
			fprintf(stderr, "%s: rsn_len got %u expect %u\n", v->name, rsn_len,
				v->expect_rsn_len);
			return -1;
		}
		if (v->expect_wpa_ie_len &&
		    memcmp(wpa_ie, v->expect_wpa_ie, v->expect_wpa_ie_len)) {
			fprintf(stderr, "%s: wpa_ie mismatch\n", v->name);
			return -1;
		}
		if (v->expect_rsn_ie_len &&
		    memcmp(rsn_ie, v->expect_rsn_ie, v->expect_rsn_ie_len)) {
			fprintf(stderr, "%s: rsn_ie mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_IS_WPS_IE: {
		unsigned int wps_ielen = 0;
		u8 match;

		if (v->null_ie_ptr) {
			match = rtw_is_wps_ie(NULL, &wps_ielen);
		} else {
			match = rtw_is_wps_ie(v->ie, &wps_ielen);
		}
		if (match != v->expect_match) {
			fprintf(stderr, "%s: match got %u expect %u\n", v->name, match,
				v->expect_match);
			return -1;
		}
		if (!v->null_ie_ptr && wps_ielen != v->expect_wps_ielen) {
			fprintf(stderr, "%s: wps_ielen got %u expect %u\n", v->name,
				wps_ielen, v->expect_wps_ielen);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "ie_rest_wapi_wps_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			failed++;
		}
	}

	if (failed)
		return 1;

	printf("PASS: %zu vectors\n", nvec);
	return 0;
}
