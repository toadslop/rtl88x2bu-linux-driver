// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for WEP ARC4/CRC32 primitives (T5 / W3-05).
 * codeql[cpp/weak-cryptographic-algorithm]: Exercises legacy WEP ARC4 via the C oracle for parity tests.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_BUF 256

enum wep_fn {
	FN_ARCFOUR = 0,
	FN_CRC32,
};

struct vector {
	char name[MAX_NAME];
	enum wep_fn fn;
	u8 key[MAX_BUF];
	size_t key_len;
	u8 data[MAX_BUF];
	size_t data_len;
	u8 expect[MAX_BUF];
	size_t expect_len;
	u32 expect_crc;
};

void host_wep_arcfour_crypt(const u8 *key, u32 key_len, const u8 *src, u8 *dest,
			    u32 len);
u32 host_wep_getcrc32(u8 *buf, int len);

static int parse_fn(const char *obj, size_t obj_len, enum wep_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_wep_arcfour_crypt") == 0)
		*out = FN_ARCFOUR;
	else if (strcmp(fn, "host_wep_getcrc32") == 0)
		*out = FN_CRC32;
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
	int crc = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &v->key_len);
	parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data), &v->data_len);
	parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
			&v->expect_len);
	if (!host_json_parse_int_in(obj, obj_len, "expect_crc", &crc))
		v->expect_crc = (u32)crc;
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_ARCFOUR: {
		u8 out[MAX_BUF];

		host_wep_arcfour_crypt(v->key, (u32)v->key_len, v->data, out,
				       (u32)v->data_len);
		if (v->expect_len != v->data_len ||
		    memcmp(out, v->expect, v->expect_len) != 0) {
			fprintf(stderr, "%s: arcfour mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_CRC32: {
		u32 got = host_wep_getcrc32(v->data, (int)v->data_len);

		if (got != v->expect_crc) {
			fprintf(stderr, "%s: crc32 mismatch got=%08x expect=%08x\n",
				v->name, got, v->expect_crc);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "wep_primitive_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu wep primitive vectors passed (oracle: core/rtw_security.c)\n",
	       nvec);
	return 0;
}
