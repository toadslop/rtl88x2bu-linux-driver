// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-omac1.c (W2-01).
 *
 * oracle: core/crypto/aes-omac1.c
 *
 * Loads fixed vectors from aes_omac1_vectors.json and runs the in-tree C
 * implementation compiled for userspace. The Rust translation links the same
 * vectors against extern "C" shims.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "aes_wrap.h"

#define MAX_VECTORS 32
#define MAX_HEX_BUF 4096
#define MAX_NAME 128
#define MAX_ELEMENTS 8
#define MAX_ELEM_LEN 1024

enum omac1_fn {
	FN_OMAC1_AES_VECTOR = 0,
	FN_OMAC1_AES_128_VECTOR,
	FN_OMAC1_AES_128,
	FN_OMAC1_AES_256,
};

struct vector {
	char name[MAX_NAME];
	enum omac1_fn fn;
	size_t key_len;
	u8 key[32];
	size_t num_elem;
	u8 elem[MAX_ELEMENTS][MAX_ELEM_LEN];
	size_t elem_len[MAX_ELEMENTS];
	u8 expected_mac[16];
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
				  const char *key, enum omac1_fn *out)
{
	char buf[64];

	if (json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
		return -1;
	if (strcmp(buf, "omac1_aes_vector") == 0) {
		*out = FN_OMAC1_AES_VECTOR;
		return 0;
	}
	if (strcmp(buf, "omac1_aes_128_vector") == 0) {
		*out = FN_OMAC1_AES_128_VECTOR;
		return 0;
	}
	if (strcmp(buf, "omac1_aes_128") == 0) {
		*out = FN_OMAC1_AES_128;
		return 0;
	}
	if (strcmp(buf, "omac1_aes_256") == 0) {
		*out = FN_OMAC1_AES_256;
		return 0;
	}
	return -1;
}

static int json_parse_elements_in(const char *obj, size_t obj_len, const char *key,
				  struct vector *v)
{
	const char *p = json_find_key_in(obj, obj_len, key);
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return -1;
	p++;
	while (count < MAX_ELEMENTS) {
		char hex[MAX_HEX_BUF];

		p = json_skip_ws(p);
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
		if (hex_decode(hex, v->elem[count], sizeof(v->elem[count]),
			       &v->elem_len[count]))
			return -1;
		count++;
		p = json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->num_elem = count;
	return count ? 0 : -1;
}

static int parse_vector_object(const char *obj, size_t obj_len, struct vector *v)
{
	char hex[MAX_HEX_BUF];
	int key_len = 0;
	size_t decoded_key_len = 0;

	memset(v, 0, sizeof(*v));
	if (json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (json_parse_int_in(obj, obj_len, "key_len", &key_len))
		return -1;
	v->key_len = (size_t)key_len;
	if (json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	if (hex_decode(hex, v->key, sizeof(v->key), &decoded_key_len))
		return -1;
	if (decoded_key_len != v->key_len)
		return -1;

	if (json_parse_string_in(obj, obj_len, "data", hex, sizeof(hex)) == 0) {
		if (hex_decode(hex, v->elem[0], sizeof(v->elem[0]), &v->elem_len[0]))
			return -1;
		v->num_elem = 1;
	} else if (json_parse_elements_in(obj, obj_len, "elements", v)) {
		return -1;
	}

	if (json_parse_string_in(obj, obj_len, "mac", hex, sizeof(hex)))
		return -1;
	{
		size_t mac_len = 0;

		if (hex_decode(hex, v->expected_mac, sizeof(v->expected_mac),
			       &mac_len) || mac_len != 16)
			return -1;
	}
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
	u8 mac[16];
	const u8 *addr[MAX_ELEMENTS];
	int ret;
	size_t i;

	for (i = 0; i < v->num_elem; i++)
		addr[i] = v->elem[i];

	switch (v->fn) {
	case FN_OMAC1_AES_VECTOR:
		ret = omac1_aes_vector(v->key, v->key_len, v->num_elem, addr,
				       v->elem_len, mac);
		break;
	case FN_OMAC1_AES_128_VECTOR:
		if (v->key_len != 16)
			return -1;
		ret = omac1_aes_128_vector(v->key, v->num_elem, addr,
					   v->elem_len, mac);
		break;
	case FN_OMAC1_AES_128:
		if (v->key_len != 16 || v->num_elem != 1)
			return -1;
		ret = omac1_aes_128(v->key, v->elem[0], v->elem_len[0], mac);
		break;
	case FN_OMAC1_AES_256:
		if (v->key_len != 32 || v->num_elem != 1)
			return -1;
		ret = omac1_aes_256(v->key, v->elem[0], v->elem_len[0], mac);
		break;
	default:
		return -1;
	}

	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
			v->expect_ret, ret);
		return -1;
	}
	if (ret == 0 && memcmp(mac, v->expected_mac, 16) != 0) {
		size_t j;

		fprintf(stderr, "%s: mac mismatch\n", v->name);
		fprintf(stderr, "  expected: ");
		for (j = 0; j < 16; j++)
			fprintf(stderr, "%02x", v->expected_mac[j]);
		fprintf(stderr, "\n  got:      ");
		for (j = 0; j < 16; j++)
			fprintf(stderr, "%02x", mac[j]);
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "aes_omac1_vectors.json";
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
	printf("all %zu aes-omac1 vectors passed (oracle: core/crypto/aes-omac1.c)\n",
	       nvec);
	return 0;
}
