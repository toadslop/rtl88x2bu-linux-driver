// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-ctr.c (T2).
 *
 * oracle: core/crypto/aes-ctr.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"
#include "aes_wrap.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	int use_aes_128_ctr;
	size_t key_len;
	u8 key[32];
	u8 nonce[16];
	u8 plaintext[1024];
	size_t plaintext_len;
	u8 expected[1024];
	size_t expected_len;
	int expect_ret;
};

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	{
		char fn[64];

		if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
			return -1;
		if (strcmp(fn, "aes_128_ctr_encrypt") == 0)
			v->use_aes_128_ctr = 1;
		else if (strcmp(fn, "aes_ctr_encrypt") == 0)
			v->use_aes_128_ctr = 0;
		else
			return -1;
	}
	{
		int key_len = 0;
		size_t decoded_key_len = 0;

		if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
			return -1;
		v->key_len = (size_t)key_len;
		if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
			return -1;
		if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded_key_len))
			return -1;
		if (decoded_key_len != v->key_len)
			return -1;
	}
	if (host_json_parse_string_in(obj, obj_len, "nonce", hex, sizeof(hex)))
		return -1;
	{
		size_t nonce_len = 0;

		if (host_hex_decode(hex, v->nonce, sizeof(v->nonce), &nonce_len) ||
		    nonce_len != 16)
			return -1;
	}
	if (host_json_parse_string_in(obj, obj_len, "plaintext", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->plaintext, sizeof(v->plaintext), &v->plaintext_len))
		return -1;
	if (host_json_parse_string_in(obj, obj_len, "ciphertext", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->expected, sizeof(v->expected), &v->expected_len))
		return -1;
	if (v->expected_len != v->plaintext_len)
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	u8 buf[1024];
	int ret;

	if (v->plaintext_len > sizeof(buf))
		return -1;
	memcpy(buf, v->plaintext, v->plaintext_len);

	if (v->use_aes_128_ctr) {
		if (v->key_len != 16)
			return -1;
		ret = aes_128_ctr_encrypt(v->key, v->nonce, buf, v->plaintext_len);
	} else {
		ret = aes_ctr_encrypt(v->key, v->key_len, v->nonce, buf,
				      v->plaintext_len);
	}

	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
			v->expect_ret, ret);
		return -1;
	}
	if (v->expected_len > 0 &&
	    memcmp(buf, v->expected, v->expected_len) != 0) {
		size_t i;

		fprintf(stderr, "%s: ciphertext mismatch\n", v->name);
		fprintf(stderr, "  expected: ");
		for (i = 0; i < v->expected_len; i++)
			fprintf(stderr, "%02x", v->expected[i]);
		fprintf(stderr, "\n  got:      ");
		for (i = 0; i < v->plaintext_len; i++)
			fprintf(stderr, "%02x", buf[i]);
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "aes_ctr_vectors.json";
	struct vector vecs[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
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
	printf("all %zu aes-ctr vectors passed (oracle: core/crypto/aes-ctr.c)\n",
	       executed);
	return 0;
}
