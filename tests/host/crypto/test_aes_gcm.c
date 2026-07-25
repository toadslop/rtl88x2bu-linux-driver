// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-gcm.c aes_gcm_ae (W2-07).
 *
 * oracle: core/crypto/aes-gcm.c (part 1); decrypt/gmac via aes-gcm_rest.c until W2-08.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"
#include "aes_wrap.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_BUF 256

enum aes_gcm_fn {
	FN_AES_GCM_AE = 0,
	FN_AES_GMAC,
};

struct vector {
	char name[MAX_NAME];
	enum aes_gcm_fn fn;
	size_t key_len;
	u8 key[32];
	u8 iv[MAX_BUF];
	size_t iv_len;
	u8 plain[MAX_BUF];
	size_t plain_len;
	u8 aad[MAX_BUF];
	size_t aad_len;
	u8 crypt[MAX_BUF];
	size_t crypt_len;
	u8 tag[16];
	int expect_ret;
	int rust_only;
};

static int json_parse_fn(const char *obj, size_t obj_len, enum aes_gcm_fn *out)
{
	char buf[32];

	if (host_json_parse_string_in(obj, obj_len, "fn", buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "aes_gcm_ae") == 0) {
		*out = FN_AES_GCM_AE;
		return 0;
	}
	if (strcmp(buf, "aes_gmac") == 0) {
		*out = FN_AES_GMAC;
		return 0;
	}
	return -1;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *out, size_t out_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, out, out_cap, out_len);
}

static int parse_hex_field_optional(const char *obj, size_t obj_len, const char *key,
				    u8 *out, size_t out_cap, size_t *out_len)
{
	if (!host_json_find_key_in(obj, obj_len, key)) {
		*out_len = 0;
		return 0;
	}
	return parse_hex_field(obj, obj_len, key, out, out_cap, out_len);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	int key_len = 0;
	size_t decoded_key_len = 0;
	size_t tag_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len) ||
	    host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	v->key_len = (size_t)key_len;
	if (parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key),
			    &decoded_key_len) ||
	    decoded_key_len != v->key_len)
		return -1;
	if (parse_hex_field(obj, obj_len, "iv", v->iv, sizeof(v->iv), &v->iv_len))
		return -1;
	if (parse_hex_field_optional(obj, obj_len, "plain", v->plain, sizeof(v->plain),
				     &v->plain_len))
		return -1;
	if (parse_hex_field_optional(obj, obj_len, "aad", v->aad, sizeof(v->aad),
				     &v->aad_len))
		return -1;
	if (parse_hex_field_optional(obj, obj_len, "crypt", v->crypt, sizeof(v->crypt),
				     &v->crypt_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "tag", v->tag, sizeof(v->tag), &tag_len) ||
	    tag_len != 16)
		return -1;
	if (v->fn == FN_AES_GCM_AE && v->expect_ret == 0 &&
	    v->crypt_len != v->plain_len)
		return -1;
	return 0;
}

static void dump_hex_mismatch(const char *label, const u8 *expected, size_t expected_len,
			      const u8 *got, size_t got_len)
{
	size_t i;

	fprintf(stderr, "  %s (%zu): ", label, expected_len);
	for (i = 0; i < expected_len; i++)
		fprintf(stderr, "%02x", expected[i]);
	fprintf(stderr, "\n  got (%zu):      ", got_len);
	for (i = 0; i < got_len; i++)
		fprintf(stderr, "%02x", got[i]);
	fprintf(stderr, "\n");
}

static int run_vector(const struct vector *v)
{
	u8 crypt[MAX_BUF];
	u8 tag[16];
	int ret;

	memset(crypt, 0x5a, sizeof(crypt));
	memset(tag, 0x5a, sizeof(tag));
	if (v->fn == FN_AES_GMAC) {
		ret = aes_gmac(v->key, v->key_len, v->iv, v->iv_len, v->aad, v->aad_len,
			       tag);
	} else {
		ret = aes_gcm_ae(v->key, v->key_len, v->iv, v->iv_len, v->plain, v->plain_len,
				 v->aad, v->aad_len, crypt, tag);
	}
	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name, v->expect_ret, ret);
		return -1;
	}
	if (ret != 0)
		return 0;
	if (v->fn == FN_AES_GCM_AE &&
	    memcmp(crypt, v->crypt, v->plain_len) != 0) {
		fprintf(stderr, "%s: crypt mismatch\n", v->name);
		dump_hex_mismatch("expected", v->crypt, v->plain_len, crypt, v->plain_len);
		return -1;
	}
	if (memcmp(tag, v->tag, 16) != 0) {
		fprintf(stderr, "%s: tag mismatch\n", v->name);
		dump_hex_mismatch("expected", v->tag, 16, tag, 16);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "aes_gcm_vectors.json";
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
#ifndef RUST_AES_GCM_ORACLE
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
		printf("all %zu aes-gcm vectors passed (%zu rust-only skipped)\n",
		       executed, skipped);
	else
		printf("all %zu aes-gcm vectors passed\n", executed);
	return 0;
}
