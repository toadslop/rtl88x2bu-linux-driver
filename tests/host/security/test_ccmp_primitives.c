// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for AES-CCMP software primitives (W3-11).
 */

#if defined(HOST_CCMP_PRIMITIVE_ORACLE_BUILD) || defined(RUST_SECURITY_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_security_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_BUF 64

enum ccmp_fn {
	FN_AES128K128D = 0,
	FN_XOR_128,
	FN_XOR_32,
	FN_BITWISE_XOR,
};

struct vector {
	char name[MAX_NAME];
	enum ccmp_fn fn;
	u8 key[16];
	u8 data[16];
	u8 input_a[16];
	u8 input_b[16];
	u8 expect[16];
};

#ifdef RUST_SECURITY_REST_ORACLE
extern void host_ccmp_aes128k128d(u8 *key, u8 *data, u8 *ciphertext);
extern void host_ccmp_xor_128(u8 *a, u8 *b, u8 *out);
extern void host_ccmp_xor_32(u8 *a, u8 *b, u8 *out);
extern void host_ccmp_bitwise_xor(u8 *ina, u8 *inb, u8 *out);
#else
void host_ccmp_aes128k128d(u8 *key, u8 *data, u8 *ciphertext);
void host_ccmp_xor_128(u8 *a, u8 *b, u8 *out);
void host_ccmp_xor_32(u8 *a, u8 *b, u8 *out);
void host_ccmp_bitwise_xor(u8 *ina, u8 *inb, u8 *out);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum ccmp_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_ccmp_aes128k128d") == 0)
		*out = FN_AES128K128D;
	else if (strcmp(fn, "host_ccmp_xor_128") == 0)
		*out = FN_XOR_128;
	else if (strcmp(fn, "host_ccmp_xor_32") == 0)
		*out = FN_XOR_32;
	else if (strcmp(fn, "host_ccmp_bitwise_xor") == 0)
		*out = FN_BITWISE_XOR;
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
	size_t len;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;

	switch (v->fn) {
	case FN_AES128K128D:
		if (parse_hex_field(obj, obj_len, "key", v->key, sizeof(v->key), &len) ||
		    len != 16)
			return -1;
		if (parse_hex_field(obj, obj_len, "data", v->data, sizeof(v->data), &len) ||
		    len != 16)
			return -1;
		break;
	case FN_XOR_128:
	case FN_BITWISE_XOR:
		if (parse_hex_field(obj, obj_len, "a", v->input_a, sizeof(v->input_a), &len) ||
		    len != 16)
			return -1;
		if (parse_hex_field(obj, obj_len, "b", v->input_b, sizeof(v->input_b), &len) ||
		    len != 16)
			return -1;
		break;
	case FN_XOR_32:
		if (parse_hex_field(obj, obj_len, "a", v->input_a, sizeof(v->input_a), &len) ||
		    len != 4)
			return -1;
		if (parse_hex_field(obj, obj_len, "b", v->input_b, sizeof(v->input_b), &len) ||
		    len != 4)
			return -1;
		break;
	}

	{
		size_t expect_len;

		switch (v->fn) {
		case FN_XOR_32:
			expect_len = 4;
			break;
		default:
			expect_len = 16;
			break;
		}
		if (parse_hex_field(obj, obj_len, "expect", v->expect, sizeof(v->expect),
				    &len) ||
		    len != expect_len)
			return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 out[16];

	switch (v->fn) {
	case FN_AES128K128D:
		host_ccmp_aes128k128d(v->key, v->data, out);
		if (memcmp(out, v->expect, 16) != 0) {
			fprintf(stderr, "%s: aes128k128d mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_XOR_128:
		host_ccmp_xor_128(v->input_a, v->input_b, out);
		if (memcmp(out, v->expect, 16) != 0) {
			fprintf(stderr, "%s: xor_128 mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_XOR_32:
		host_ccmp_xor_32(v->input_a, v->input_b, out);
		if (memcmp(out, v->expect, 4) != 0) {
			fprintf(stderr, "%s: xor_32 mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_BITWISE_XOR:
		host_ccmp_bitwise_xor(v->input_a, v->input_b, out);
		if (memcmp(out, v->expect, 16) != 0) {
			fprintf(stderr, "%s: bitwise_xor mismatch\n", v->name);
			return -1;
		}
		break;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "ccmp_primitive_vectors.json";
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
	printf("all %zu ccmp primitive vectors passed (oracle: core/rtw_security_rest.c)\n",
	       nvec);
	return 0;
}

#endif /* HOST_CCMP_PRIMITIVE_ORACLE_BUILD || RUST_SECURITY_REST_ORACLE */
