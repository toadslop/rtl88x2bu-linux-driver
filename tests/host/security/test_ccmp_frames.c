// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for AES-CCMP frame encrypt/decrypt (W3-12/W3-13 / T5).
 */

#if defined(HOST_CCMP_FRAME_ORACLE_BUILD) || defined(RUST_SECURITY_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

typedef int sint;

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_BUF 256

enum ccmp_frame_fn {
	FN_AES_CIPHER = 0,
	FN_AES_DECIPHER,
};

struct vector {
	char name[MAX_NAME];
	enum ccmp_frame_fn fn;
	u8 key[16];
	size_t key_len;
	u16 hdrlen;
	u32 plen;
	u8 frame[MAX_BUF];
	size_t frame_len;
	u8 expect[MAX_BUF];
	size_t expect_len;
	int expect_fail;
};

#ifdef RUST_SECURITY_REST_ORACLE
extern sint host_ccmp_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe,
				 unsigned int plen);
extern sint host_ccmp_aes_decipher(u8 *key, unsigned int hdrlen, u8 *pframe,
				   unsigned int plen);
#else
sint host_ccmp_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe,
			  unsigned int plen);
sint host_ccmp_aes_decipher(u8 *key, unsigned int hdrlen, u8 *pframe,
			    unsigned int plen);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum ccmp_frame_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "aes_cipher") == 0)
		*out = FN_AES_CIPHER;
	else if (strcmp(fn, "aes_decipher") == 0)
		*out = FN_AES_DECIPHER;
	else
		return -1;
	return 0;
}

static int parse_hex_field(const char *obj, size_t obj_len, const char *key,
			   u8 *out, size_t out_cap, size_t *out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	if (host_json_parse_string_in(obj, obj_len, key, hex, sizeof(hex)))
		return -1;
	return host_hex_decode(hex, out, out_cap, out_len);
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	size_t key_len = 0;
	int val = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &key_len))
		return -1;
	v->key_len = key_len;
	if (host_json_parse_int_in(obj, obj_len, "hdrlen", &val))
		return -1;
	v->hdrlen = (u16)val;
	if (host_json_parse_int_in(obj, obj_len, "plen", &val))
		return -1;
	v->plen = (u32)val;
	if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame),
			    &v->frame_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
			    &v->expect_len))
		return -1;
	if (!host_json_parse_int_in(obj, obj_len, "expect_fail", &val))
		v->expect_fail = val;
	return 0;
}

static int run_cipher_vector(struct vector *v)
{
	u8 buf[MAX_BUF];
	sint ret;
	size_t need;

	if (v->key_len != 16) {
		fprintf(stderr, "%s: key_len %zu != 16\n", v->name, v->key_len);
		return -1;
	}

	need = (size_t)v->hdrlen + 8 + (size_t)v->plen + 8;
	if (v->frame_len < need) {
		fprintf(stderr, "%s: frame_len %zu < hdrlen+pn+plen+mic (%zu)\n",
			v->name, v->frame_len, need);
		return -1;
	}

	if (v->expect_len != v->plen + 8) {
		fprintf(stderr, "%s: expect_len %zu != plen+8 (%u)\n", v->name,
			v->expect_len, v->plen + 8);
		return -1;
	}

	if (v->frame_len > MAX_BUF)
		return -1;
	memcpy(buf, v->frame, v->frame_len);

	ret = host_ccmp_aes_cipher(v->key, v->hdrlen, buf, v->plen);
	if (ret != 1) {
		fprintf(stderr, "%s: aes_cipher returned %d\n", v->name, ret);
		return -1;
	}

	if (memcmp(buf + v->hdrlen + 8, v->expect, v->expect_len) != 0) {
		fprintf(stderr, "%s: output mismatch\n", v->name);
		return -1;
	}

	return 0;
}

static int run_decipher_vector(struct vector *v)
{
	u8 buf[MAX_BUF];
	sint ret;
	size_t need;
	u32 plain_len;

	if (v->key_len != 16) {
		fprintf(stderr, "%s: key_len %zu != 16\n", v->name, v->key_len);
		return -1;
	}

	if (v->plen < 8) {
		fprintf(stderr, "%s: plen %u < 8 (must include MIC)\n", v->name, v->plen);
		return -1;
	}

	plain_len = v->plen - 8;
	need = (size_t)v->hdrlen + 8 + (size_t)v->plen;
	if (v->frame_len < need) {
		fprintf(stderr, "%s: frame_len %zu < hdrlen+pn+plen (%zu)\n",
			v->name, v->frame_len, need);
		return -1;
	}

	if (v->expect_len != plain_len) {
		fprintf(stderr, "%s: expect_len %zu != plen-8 (%u)\n", v->name,
			v->expect_len, plain_len);
		return -1;
	}

	if (v->frame_len > MAX_BUF)
		return -1;
	memcpy(buf, v->frame, v->frame_len);

	ret = host_ccmp_aes_decipher(v->key, v->hdrlen, buf, v->plen);
	if (v->expect_fail) {
		if (ret == 1) {
			fprintf(stderr, "%s: expected failure, got success\n", v->name);
			return -1;
		}
		return 0;
	}

	if (ret != 1) {
		fprintf(stderr, "%s: aes_decipher returned %d\n", v->name, ret);
		return -1;
	}

	if (memcmp(buf + v->hdrlen + 8, v->expect, v->expect_len) != 0) {
		fprintf(stderr, "%s: output mismatch\n", v->name);
		return -1;
	}

	return 0;
}

static int run_vector(struct vector *v)
{
	if (v->fn == FN_AES_CIPHER)
		return run_cipher_vector(v);
	if (v->fn == FN_AES_DECIPHER)
		return run_decipher_vector(v);
	return -1;
}

int main(int argc, char **argv)
{
	const char *path = "ccmp_frame_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (size_t i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "vector %s failed\n", vectors[i].name);
			return 1;
		}
	}

	printf("all %zu ccmp frame vectors passed (oracle: core/rtw_security_rest.c)\n",
	       nvec);
	return 0;
}

#endif /* HOST_CCMP_FRAME_ORACLE_BUILD || RUST_SECURITY_REST_ORACLE */
