// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-siv.c (W2-03a).
 *
 * oracle: core/crypto/aes-siv.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"
#include "aes_siv.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_ELEMENTS 6
#define MAX_ELEM_LEN 1024
#define MAX_PW_LEN 1024
#define MAX_OUT_LEN 2048

enum aes_siv_fn {
	FN_AES_SIV_ENCRYPT = 0,
	FN_AES_SIV_DECRYPT,
};

struct vector {
	char name[MAX_NAME];
	enum aes_siv_fn fn;
	size_t key_len;
	u8 key[64];
	size_t num_elem;
	u8 elem[MAX_ELEMENTS][MAX_ELEM_LEN];
	size_t elem_len[MAX_ELEMENTS];
	u8 pw[MAX_PW_LEN];
	size_t pwlen;
	u8 iv_crypt[MAX_OUT_LEN];
	size_t iv_crypt_len;
	u8 expected[MAX_OUT_LEN];
	size_t expected_len;
	int expect_ret;
	int rust_only;
};

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, enum aes_siv_fn *out)
{
	char buf[64];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "aes_siv_encrypt") == 0) {
		*out = FN_AES_SIV_ENCRYPT;
		return 0;
	}
	if (strcmp(buf, "aes_siv_decrypt") == 0) {
		*out = FN_AES_SIV_DECRYPT;
		return 0;
	}
	return -1;
}

static int json_parse_elements_in(const char *obj, size_t obj_len, const char *key,
				  struct vector *v)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return 0;
	p++;
	while (count < MAX_ELEMENTS) {
		char hex[HOST_VECTOR_MAX_HEX_BUF];

		p = host_json_skip_ws(p);
		if (*p == ']')
			break;
		if (*p != '"')
			return -1;
		{
			const char *start = p + 1;
			const char *end = strchr(start, '"');

			if (!end || (size_t)(end - start) + 1 >= sizeof(hex))
				return -1;
			memcpy(hex, start, (size_t)(end - start));
			hex[end - start] = '\0';
			p = end + 1;
		}
		if (host_hex_decode(hex, v->elem[count], sizeof(v->elem[count]),
				    &v->elem_len[count]))
			return -1;
		count++;
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->num_elem = count;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	int key_len = 0;
	size_t decoded_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
		return -1;
	v->key_len = (size_t)key_len;
	if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded_len) ||
	    decoded_len != v->key_len)
		return -1;
	if (json_parse_elements_in(obj, obj_len, "elements", v))
		return -1;

	if (v->fn == FN_AES_SIV_ENCRYPT) {
		if (host_json_parse_string_in(obj, obj_len, "pw", hex, sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->pw, sizeof(v->pw), &v->pwlen))
			return -1;
		if (host_json_parse_string_in(obj, obj_len, "expected", hex,
					      sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->expected, sizeof(v->expected),
				    &v->expected_len))
			return -1;
	} else {
		if (host_json_parse_string_in(obj, obj_len, "iv_crypt", hex,
					      sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->iv_crypt, sizeof(v->iv_crypt),
				    &v->iv_crypt_len))
			return -1;
		if (host_json_parse_string_in(obj, obj_len, "expected_pw", hex,
					      sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->expected, sizeof(v->expected),
				    &v->expected_len))
			return -1;
	}

	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	const u8 *addr[MAX_ELEMENTS];
	size_t i;
	int ret;

	for (i = 0; i < v->num_elem; i++)
		addr[i] = v->elem[i];

	if (v->fn == FN_AES_SIV_ENCRYPT) {
		u8 out[MAX_OUT_LEN];

		ret = aes_siv_encrypt(v->key, v->key_len, v->pw, v->pwlen,
				      v->num_elem, addr, v->elem_len, out);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
		if (ret == 0) {
			size_t out_len = 16 + v->pwlen;

			if (out_len != v->expected_len ||
			    memcmp(out, v->expected, out_len) != 0) {
				fprintf(stderr, "%s: encrypt output mismatch\n",
					v->name);
				return -1;
			}
		}
		return 0;
	}

	{
		u8 out[MAX_PW_LEN];

		ret = aes_siv_decrypt(v->key, v->key_len, v->iv_crypt,
				      v->iv_crypt_len, v->num_elem, addr,
				      v->elem_len, out);
		if (ret != v->expect_ret) {
			fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
				v->expect_ret, ret);
			return -1;
		}
		if (ret == 0) {
			size_t plain_len = v->iv_crypt_len - 16;

			if (plain_len != v->expected_len ||
			    memcmp(out, v->expected, plain_len) != 0) {
				fprintf(stderr, "%s: decrypt plaintext mismatch\n",
					v->name);
				return -1;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "aes_siv_vectors.json";
	struct vector vecs[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	size_t skipped = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
#ifndef RUST_AES_SIV_ORACLE
		if (vecs[i].rust_only) {
			printf("skip %s (rust-only)\n", vecs[i].name);
			skipped++;
			continue;
		}
#endif
		executed++;
		if (run_vector(&vecs[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vecs[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	if (skipped)
		printf("all %zu aes-siv vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/aes-siv.c)\n",
		       executed, skipped);
	else
		printf("all %zu aes-siv vectors passed "
		       "(oracle: core/crypto/aes-siv.c)\n",
		       executed);
	return 0;
}
