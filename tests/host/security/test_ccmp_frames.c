// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for AES-CCMP frame encrypt (W3-12 / T5).
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
};

#ifdef RUST_SECURITY_REST_ORACLE
extern sint host_ccmp_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe,
				 unsigned int plen);
#else
sint host_ccmp_aes_cipher(u8 *key, unsigned int hdrlen, u8 *pframe,
			  unsigned int plen);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum ccmp_frame_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "aes_cipher") == 0)
		*out = FN_AES_CIPHER;
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

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &key_len))
		return -1;
	v->key_len = key_len;
	{
		int val = 0;
		if (host_json_parse_int_in(obj, obj_len, "hdrlen", &val))
			return -1;
		v->hdrlen = (u16)val;
		if (host_json_parse_int_in(obj, obj_len, "plen", &val))
			return -1;
		v->plen = (u32)val;
	}
	if (parse_hex_field(obj, obj_len, "frame", v->frame, sizeof(v->frame),
			    &v->frame_len))
		return -1;
	if (parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
			    &v->expect_len))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 buf[MAX_BUF];
	sint ret;

	if (v->frame_len > MAX_BUF)
		return -1;
	memcpy(buf, v->frame, v->frame_len);

	if (v->fn != FN_AES_CIPHER)
		return -1;

	ret = host_ccmp_aes_cipher(v->key, v->hdrlen, buf, v->plen);
	if (ret != 1) {
		fprintf(stderr, "%s: aes_cipher returned %d\n", v->name, ret);
		return -1;
	}

	if (v->hdrlen + 8 + v->plen + 8 > v->frame_len) {
		fprintf(stderr, "%s: frame buffer too small for output region\n",
			v->name);
		return -1;
	}

	if (v->expect_len != v->plen + 8) {
		fprintf(stderr, "%s: expect_len %zu != plen+8 (%u)\n", v->name,
			v->expect_len, v->plen + 8);
		return -1;
	}

	if (memcmp(buf + v->hdrlen + 8, v->expect, v->expect_len) != 0) {
		fprintf(stderr, "%s: output mismatch\n", v->name);
		return -1;
	}

	return 0;
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
