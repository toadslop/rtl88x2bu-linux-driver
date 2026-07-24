// SPDX-License-Identifier: GPL-2.0
/*
 * GCMP vector parse + oracle runner (W2-02b).
 */

#include <stdio.h>
#include <string.h>

#include "host_gcmp_vector.h"
#include "host_vector_json.h"
#include "rtw_crypto_wrap.h"
#include "wlancrypto_wrap.h"

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, enum host_gcmp_fn *out)
{
	char buf[64];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "gcmp_encrypt") == 0) {
		*out = HOST_GCMP_FN_ENCRYPT;
		return 0;
	}
	if (strcmp(buf, "gcmp_decrypt") == 0) {
		*out = HOST_GCMP_FN_DECRYPT;
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

int host_gcmp_parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct host_gcmp_vector *v = vec_void;
	int key_len = 0;
	int amsdu_mode = 0;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	if (host_json_parse_int_in(obj, obj_len, "amsdu_mode", &amsdu_mode))
		return -1;
	v->amsdu_mode = amsdu_mode;
	if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
		return -1;
	v->key_len = (size_t)key_len;
	if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	{
		size_t decoded = 0;

		if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded))
			return -1;
		if (decoded != v->key_len)
			return -1;
	}
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;

	if (v->fn == HOST_GCMP_FN_ENCRYPT) {
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
			v->has_pn = 1;
		}
		if (host_json_parse_int_in(obj, obj_len, "keyid", &v->keyid))
			return -1;
		if (v->expect_ret &&
		    parse_hex_field(obj, obj_len, "ciphertext", v->expected,
				    sizeof(v->expected), &v->expected_len))
			return -1;
	} else {
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
	}
	return 0;
}

int host_gcmp_run_vector(const struct host_gcmp_vector *v)
{
	_adapter adapter = { .registrypriv = { .amsdu_mode = (u8)v->amsdu_mode } };
	size_t out_len = 0;
	u8 *out = NULL;
	int ok;

	switch (v->fn) {
	case HOST_GCMP_FN_ENCRYPT:
		out = gcmp_encrypt(&adapter, v->key, v->key_len, v->frame, v->frame_len,
				   v->hdrlen, NULL,
				   v->null_pn ? NULL : (v->has_pn ? v->pn : NULL),
				   v->keyid, &out_len);
		break;
	case HOST_GCMP_FN_DECRYPT:
		out = gcmp_decrypt(&adapter, v->key, v->key_len,
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
