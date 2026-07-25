// SPDX-License-Identifier: GPL-2.0
/*
 * AES-internal vector parse + oracle runner (W2-11a).
 */

#include <stdio.h>
#include <string.h>

#include "host_types.h"
#include "aes.h"
#include "aes_i.h"
#include "host_aes_internal_vector.h"
#include "host_vector_json.h"

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, enum host_aes_internal_fn *out)
{
	char buf[64];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "rijndaelKeySetupEnc") == 0) {
		*out = HOST_AES_INTERNAL_FN_KEY_SETUP_ENC;
		return 0;
	}
	if (strcmp(buf, "aes_encrypt") == 0) {
		*out = HOST_AES_INTERNAL_FN_ENCRYPT;
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

int host_aes_internal_parse_vector_object(const char *obj, size_t obj_len,
					  void *vec_void)
{
	struct host_aes_internal_vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int key_len = 0;
	int key_bits = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;

	switch (v->fn) {
	case HOST_AES_INTERNAL_FN_KEY_SETUP_ENC:
		if (host_json_parse_int_in(obj, obj_len, "key_bits", &key_bits))
			return -1;
		v->key_bits = key_bits;
		if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded))
			return -1;
		v->key_len = decoded;
		if (host_json_parse_int_in(obj, obj_len, "expect_rounds",
					   &v->expect_rounds))
			return -1;
		if (v->expect_ret && v->expect_rounds > 0 &&
		    parse_hex_field(obj, obj_len, "rk", v->expected_rk,
				    sizeof(v->expected_rk), &v->expected_rk_len))
			return -1;
		return 0;
	case HOST_AES_INTERNAL_FN_ENCRYPT:
		if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
			return -1;
		v->key_len = (size_t)key_len;
		if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded))
			return -1;
		if (decoded != v->key_len)
			return -1;
		if (parse_hex_field(obj, obj_len, "plaintext", v->plaintext,
				    sizeof(v->plaintext), &v->plaintext_len))
			return -1;
		if (v->plaintext_len != 16)
			return -1;
		if (v->expect_ret &&
		    parse_hex_field(obj, obj_len, "ciphertext", v->expected,
				    sizeof(v->expected), &v->expected_len))
			return -1;
		return v->expected_len == 16 ? 0 : -1;
	default:
		return -1;
	}
}

int host_aes_internal_run_vector(const struct host_aes_internal_vector *v)
{
	switch (v->fn) {
	case HOST_AES_INTERNAL_FN_KEY_SETUP_ENC: {
		u32 rk[4 * 15];
		int rounds;
		size_t expected_words;
		size_t i;

		rounds = rijndaelKeySetupEnc(rk, v->key, v->key_bits);
		if (rounds != v->expect_rounds) {
			fprintf(stderr, "%s: expected rounds %d, got %d\n",
				v->name, v->expect_rounds, rounds);
			return -1;
		}
		if (rounds < 0)
			return v->expect_ret ? 0 : -1;

		expected_words = (size_t)(rounds + 1) * 4;
		if (v->expected_rk_len != expected_words * 4) {
			fprintf(stderr, "%s: rk length mismatch (%zu vs %zu)\n",
				v->name, v->expected_rk_len, expected_words * 4);
			return -1;
		}
		for (i = 0; i < expected_words; i++) {
			u8 got[4];

			got[0] = (rk[i] >> 24) & 0xff;
			got[1] = (rk[i] >> 16) & 0xff;
			got[2] = (rk[i] >> 8) & 0xff;
			got[3] = rk[i] & 0xff;
			if (memcmp(got, v->expected_rk + i * 4, 4) != 0) {
				fprintf(stderr, "%s: rk word %zu mismatch\n",
					v->name, i);
				return -1;
			}
		}
		return v->expect_ret ? 0 : -1;
	}
	case HOST_AES_INTERNAL_FN_ENCRYPT: {
		void *ctx;
		u8 crypt[16];
		int ok;

		ctx = aes_encrypt_init(v->key, v->key_len);
		ok = ctx != NULL;
		if (ok != v->expect_ret) {
			fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
				v->expect_ret, ok);
			if (ctx)
				aes_encrypt_deinit(ctx);
			return -1;
		}
		if (!ok)
			return 0;

		if (aes_encrypt(ctx, v->plaintext, crypt) != 0) {
			aes_encrypt_deinit(ctx);
			fprintf(stderr, "%s: aes_encrypt failed\n", v->name);
			return -1;
		}
		aes_encrypt_deinit(ctx);

		if (memcmp(crypt, v->expected, 16) != 0) {
			size_t i;

			fprintf(stderr, "%s: ciphertext mismatch\n", v->name);
			fprintf(stderr, "  expected: ");
			for (i = 0; i < 16; i++)
				fprintf(stderr, "%02x", v->expected[i]);
			fprintf(stderr, "\n  got:      ");
			for (i = 0; i < 16; i++)
				fprintf(stderr, "%02x", crypt[i]);
			fprintf(stderr, "\n");
			return -1;
		}
		return 0;
	}
	default:
		return -1;
	}
}
