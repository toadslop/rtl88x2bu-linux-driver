// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-ctr.c (T2).
 *
 * oracle: core/crypto/aes-ctr.c
 *
 * Loads fixed vectors from aes_ctr_vectors.json and runs the in-tree C
 * implementation compiled for userspace. W1-03 will link the same vectors
 * against the Rust translation + extern "C" shim.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "aes_wrap.h"

#define MAX_VECTORS 32
#define MAX_HEX_BUF 4096
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

static int hex_nibble(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

static int hex_decode(const char *hex, u8 *out, size_t out_cap, size_t *out_len)
{
	size_t n = 0;
	int hi = -1;

	if (!hex)
		return -1;

	for (; *hex; hex++) {
		int v;

		if (*hex == ' ' || *hex == '\n' || *hex == '\r' || *hex == '\t')
			continue;
		v = hex_nibble(*hex);
		if (v < 0)
			return -1;
		if (hi < 0) {
			hi = v;
			continue;
		}
		if (n >= out_cap)
			return -1;
		out[n++] = (u8)((hi << 4) | v);
		hi = -1;
	}
	if (hi >= 0)
		return -1;
	*out_len = n;
	return 0;
}

static const char *json_skip_ws(const char *p)
{
	while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
		p++;
	return p;
}

static const char *json_find_key_in(const char *obj, size_t obj_len, const char *key)
{
	char pattern[64];
	const char *end = obj + obj_len;
	const char *p;
	size_t key_len = strlen(key);

	snprintf(pattern, sizeof(pattern), "\"%s\"", key);
	for (p = obj; p < end; p++) {
		if (strncmp(p, pattern, key_len + 2) != 0)
			continue;
		p += key_len + 2;
		p = json_skip_ws(p);
		if (p >= end || *p != ':')
			return NULL;
		p++;
		return json_skip_ws(p);
	}
	return NULL;
}

static int json_parse_string_in(const char *obj, size_t obj_len, const char *key,
				char *out, size_t out_cap)
{
	const char *p = json_find_key_in(obj, obj_len, key);
	size_t i = 0;

	if (!p || p >= obj + obj_len || *p != '"')
		return -1;
	p++;
	while (p < obj + obj_len && *p && *p != '"') {
		if (i + 1 >= out_cap)
			return -1;
		out[i++] = *p++;
	}
	if (p >= obj + obj_len || *p != '"')
		return -1;
	out[i] = '\0';
	return 0;
}

static int json_parse_int_in(const char *obj, size_t obj_len, const char *key,
			     int *out)
{
	const char *p = json_find_key_in(obj, obj_len, key);
	char *end = NULL;
	long v;

	if (!p || p >= obj + obj_len)
		return -1;
	v = strtol(p, &end, 10);
	if (end == p)
		return -1;
	*out = (int)v;
	return 0;
}

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, int *out)
{
	char buf[64];

	if (json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "aes_128_ctr_encrypt") == 0) {
		*out = 1;
		return 0;
	}
	if (strcmp(buf, "aes_ctr_encrypt") == 0) {
		*out = 0;
		return 0;
	}
	return -1;
}

static int parse_vector_object(const char *obj, size_t obj_len, struct vector *v)
{
	char hex[MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->use_aes_128_ctr))
		return -1;
	{
		int key_len = 0;
		size_t decoded_key_len = 0;

		if (json_parse_int_in(obj, obj_len, "key_len", &key_len))
			return -1;
		v->key_len = (size_t)key_len;
		if (json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
			return -1;
		if (hex_decode(hex, v->key, sizeof(v->key), &decoded_key_len))
			return -1;
		if (decoded_key_len != v->key_len)
			return -1;
	}
	if (json_parse_string_in(obj, obj_len, "nonce", hex, sizeof(hex)))
		return -1;
	{
		size_t nonce_len = 0;
		if (hex_decode(hex, v->nonce, sizeof(v->nonce), &nonce_len) ||
		    nonce_len != 16)
			return -1;
	}
	if (json_parse_string_in(obj, obj_len, "plaintext", hex, sizeof(hex)))
		return -1;
	if (hex_decode(hex, v->plaintext, sizeof(v->plaintext), &v->plaintext_len))
		return -1;
	if (json_parse_string_in(obj, obj_len, "ciphertext", hex, sizeof(hex)))
		return -1;
	if (hex_decode(hex, v->expected, sizeof(v->expected), &v->expected_len))
		return -1;
	if (v->expected_len != v->plaintext_len)
		return -1;
	if (json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	return 0;
}

static int load_vectors(const char *path, struct vector *vecs, size_t cap,
			size_t *count_out)
{
	char *json = NULL;
	long fsize;
	size_t count = 0;
	const char *p;
	FILE *f = fopen(path, "rb");

	if (!f)
		return -1;
	if (fseek(f, 0, SEEK_END))
		goto fail;
	fsize = ftell(f);
	if (fsize < 0)
		goto fail;
	rewind(f);
	json = malloc((size_t)fsize + 1);
	if (!json)
		goto fail;
	if (fread(json, 1, (size_t)fsize, f) != (size_t)fsize)
		goto fail;
	json[fsize] = '\0';
	fclose(f);
	f = NULL;

	p = strstr(json, "\"vectors\"");
	if (!p)
		goto fail;
	p = strchr(p, '[');
	if (!p)
		goto fail;
	p++;
	while (count < cap) {
		p = json_skip_ws(p);
		if (*p == ']')
			break;
		if (*p != '{')
			goto fail;
		{
			const char *obj = p;
			const char *obj_end = NULL;
			int depth = 0;

			while (*p) {
				if (*p == '{')
					depth++;
				else if (*p == '}') {
					depth--;
					if (depth == 0) {
						obj_end = p + 1;
						p++;
						break;
					}
				}
				p++;
			}
			if (!obj_end)
				goto fail;
			if (parse_vector_object(obj, (size_t)(obj_end - obj),
						&vecs[count]))
				goto fail;
			count++;
		}
		p = json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	free(json);
	*count_out = count;
	return 0;

fail:
	free(json);
	if (f)
		fclose(f);
	return -1;
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
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (load_vectors(path, vecs, MAX_VECTORS, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
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
	       nvec);
	return 0;
}
