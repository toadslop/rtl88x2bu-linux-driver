// SPDX-License-Identifier: GPL-2.0
/*
 * CCMP vector parse + oracle runner (W2-09a).
 */

#include <stdio.h>
#include <string.h>

#include "host_ccmp_vector.h"
#include "host_vector_json.h"
#include "rtw_crypto_wrap.h"
#include "wlancrypto_wrap.h"

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, enum host_ccmp_fn *out)
{
	char buf[64];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "ccmp_encrypt") == 0) {
		*out = HOST_CCMP_FN_ENCRYPT;
		return 0;
	}
	if (strcmp(buf, "ccmp_decrypt") == 0) {
		*out = HOST_CCMP_FN_DECRYPT;
		return 0;
	}
	if (strcmp(buf, "ccmp_encrypt_pv1") == 0) {
		*out = HOST_CCMP_FN_ENCRYPT_PV1;
		return 0;
	}
	if (strcmp(buf, "ccmp_256_encrypt") == 0) {
		*out = HOST_CCMP_FN_256_ENCRYPT;
		return 0;
	}
	if (strcmp(buf, "ccmp_256_decrypt") == 0) {
		*out = HOST_CCMP_FN_256_DECRYPT;
		return 0;
	}
	return -1;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *buf, size_t buf_sz, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, buf, buf_sz, out_len))
		return -1;
	return 0;
}

static int parse_mac_field(const char *obj, size_t obj_len, const char *key,
			   u8 *mac)
{
	size_t len = 0;

	return parse_hex_field(obj, obj_len, key, mac, 6, &len) || len != 6;
}

static int parse_encrypt_fields(struct host_ccmp_vector *v, const char *obj,
				size_t obj_len)
{
	if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame),
			    &v->frame_len))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "hdrlen", (int *)&v->hdrlen))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "null_pn", &v->null_pn) != 0)
		v->null_pn = 0;
	if (!v->null_pn) {
		size_t decoded_pn_len = 0;

		if (parse_hex_field(obj, obj_len, "pn", v->pn, sizeof(v->pn),
				    &decoded_pn_len))
			return -1;
		if (decoded_pn_len != 6)
			return -1;
	}
	if (host_json_parse_int_in(obj, obj_len, "keyid", &v->keyid))
		return -1;
	if (v->expect_ret &&
	    parse_hex_field(obj, obj_len, "ciphertext", v->expected,
			    sizeof(v->expected), &v->expected_len))
		return -1;
	return 0;
}

int host_ccmp_parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct host_ccmp_vector *v = vec_void;
	int amsdu_mode = 0;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;

	if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded))
		return -1;

	switch (v->fn) {
	case HOST_CCMP_FN_ENCRYPT:
	case HOST_CCMP_FN_256_ENCRYPT:
		if (host_json_parse_int_in(obj, obj_len, "amsdu_mode", &amsdu_mode))
			return -1;
		if (amsdu_mode < RTW_AMSDU_MODE_NON_SPP ||
		    amsdu_mode > RTW_AMSDU_MODE_ALL_DROP)
			return -1;
		v->amsdu_mode = (enum rtw_amsdu_mode)amsdu_mode;
		return parse_encrypt_fields(v, obj, obj_len);
	case HOST_CCMP_FN_ENCRYPT_PV1:
		if (parse_mac_field(obj, obj_len, "a1", v->a1) ||
		    parse_mac_field(obj, obj_len, "a2", v->a2))
			return -1;
		if (host_json_parse_bool_in(obj, obj_len, "has_a3", &v->has_a3) != 0)
			v->has_a3 = 0;
		if (v->has_a3 && parse_mac_field(obj, obj_len, "a3", v->a3))
			return -1;
		return parse_encrypt_fields(v, obj, obj_len);
	case HOST_CCMP_FN_DECRYPT:
	case HOST_CCMP_FN_256_DECRYPT:
		if (host_json_parse_int_in(obj, obj_len, "amsdu_mode", &amsdu_mode))
			return -1;
		if (amsdu_mode < RTW_AMSDU_MODE_NON_SPP ||
		    amsdu_mode > RTW_AMSDU_MODE_ALL_DROP)
			return -1;
		v->amsdu_mode = (enum rtw_amsdu_mode)amsdu_mode;
		if (parse_hex_field(obj, obj_len, "hdr", v->hdr, sizeof(v->hdr),
				    &v->hdr_len))
			return -1;
		if (parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data),
				    &v->data_len))
			return -1;
		if (v->expect_ret &&
		    parse_hex_field(obj, obj_len, "plaintext", v->expected,
				    sizeof(v->expected), &v->expected_len))
			return -1;
		return 0;
	default:
		return -1;
	}
}

int host_ccmp_run_vector(const struct host_ccmp_vector *v)
{
	_adapter adapter;
	size_t out_len = 0;
	u8 *out = NULL;
	int ok;

	memset(&adapter, 0, sizeof(adapter));
	host_adapter_set_amsdu_mode(&adapter, v->amsdu_mode);

	switch (v->fn) {
#ifndef RUST_CCMP_ORACLE
	case HOST_CCMP_FN_ENCRYPT: {
		size_t frame_len = v->frame_len;

		if (v->null_pn) {
			if (frame_len < v->hdrlen + 8)
				return -1;
			frame_len -= 8;
		}
		out = ccmp_encrypt(&adapter, v->key, (u8 *)v->frame, frame_len,
				   v->hdrlen, NULL,
				   v->null_pn ? NULL : (u8 *)v->pn,
				   v->keyid, &out_len);
		break;
	}
	case HOST_CCMP_FN_256_ENCRYPT: {
		size_t frame_len = v->frame_len;

		if (v->null_pn) {
			if (frame_len < v->hdrlen + 8)
				return -1;
			frame_len -= 8;
		}
		out = ccmp_256_encrypt(&adapter, v->key, (u8 *)v->frame, frame_len,
				       v->hdrlen, NULL,
				       v->null_pn ? NULL : (u8 *)v->pn,
				       v->keyid, &out_len);
		break;
	}
	case HOST_CCMP_FN_ENCRYPT_PV1:
		out = ccmp_encrypt_pv1(v->key, v->a1, v->a2,
				       v->has_a3 ? v->a3 : NULL,
				       v->frame, v->frame_len, v->hdrlen,
				       v->pn, v->keyid, &out_len);
		break;
#endif
	case HOST_CCMP_FN_DECRYPT:
		if (v->hdr_len < 24)
			return -1;
		out = ccmp_decrypt(&adapter, v->key,
				   (const struct ieee80211_hdr *)v->hdr,
				   v->data, v->data_len, &out_len);
		break;
	case HOST_CCMP_FN_256_DECRYPT:
		if (v->hdr_len < 24)
			return -1;
		out = ccmp_256_decrypt(&adapter, v->key,
				       (const struct ieee80211_hdr *)v->hdr,
				       v->data, v->data_len, &out_len);
		break;
	default:
		return -1;
	}

	ok = (out != NULL) ? 1 : 0;
	if (ok != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
			v->expect_ret, ok);
		if (out)
			rtw_mfree(out, out_len + 16);
		return -1;
	}
	if (!ok)
		return 0;

	if (out_len != v->expected_len ||
	    memcmp(out, v->expected, v->expected_len) != 0) {
		size_t i;

		fprintf(stderr, "%s: output mismatch\n", v->name);
		fprintf(stderr, "  expected (%zu): ", v->expected_len);
		for (i = 0; i < v->expected_len; i++)
			fprintf(stderr, "%02x", v->expected[i]);
		fprintf(stderr, "\n  got (%zu):      ", out_len);
		for (i = 0; i < out_len; i++)
			fprintf(stderr, "%02x", out[i]);
		fprintf(stderr, "\n");
		rtw_mfree(out, out_len + 16);
		return -1;
	}

	rtw_mfree(out, out_len + 16);
	return 0;
}
