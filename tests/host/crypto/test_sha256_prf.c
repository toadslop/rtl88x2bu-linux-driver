// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for sha256-prf.c (W2-06a).
 *
 * oracle: core/crypto/sha256-prf.c (+ sha256.c for hmac_sha256_vector)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_KEY_LEN 64
#define MAX_DATA_LEN 256
#define MAX_OUT_LEN 128
#define MAX_LABEL 64

enum prf_fn {
	FN_SHA256_PRF = 0,
	FN_SHA256_PRF_BITS,
};

struct vector {
	char name[MAX_NAME];
	enum prf_fn fn;
	u8 key[MAX_KEY_LEN];
	size_t key_len;
	char label[MAX_LABEL];
	u8 data[MAX_DATA_LEN];
	size_t data_len;
	size_t buf_len;
	size_t buf_len_bits;
	u8 expected_out[MAX_OUT_LEN];
	size_t expected_out_len;
	int expect_ret;
	int rust_only;
};

int sha256_prf(const u8 *key, size_t key_len, const char *label,
	       const u8 *data, size_t data_len, u8 *buf, size_t buf_len);
int sha256_prf_bits(const u8 *key, size_t key_len, const char *label,
		    const u8 *data, size_t data_len, u8 *buf,
		    size_t buf_len_bits);

static int parse_fn(const char *obj, size_t obj_len, enum prf_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "sha256_prf") == 0) {
		*out = FN_SHA256_PRF;
		return 0;
	}
	if (strcmp(fn, "sha256_prf_bits") == 0) {
		*out = FN_SHA256_PRF_BITS;
		return 0;
	}
	return -1;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;

	if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->key, sizeof(v->key), &v->key_len))
		return -1;

	if (host_json_parse_string_in(obj, obj_len, "label", v->label,
				      sizeof(v->label)))
		return -1;

	if (host_json_parse_string_in(obj, obj_len, "data", hex, sizeof(hex)) == 0) {
		if (*hex) {
			if (host_hex_decode(hex, v->data, sizeof(v->data),
					    &v->data_len))
				return -1;
		}
	}

	if (v->fn == FN_SHA256_PRF) {
		int bl = 0;

		if (host_json_parse_int_in(obj, obj_len, "buf_len", &bl))
			return -1;
		v->buf_len = (size_t)bl;
		v->expected_out_len = v->buf_len;
	} else {
		int blb = 0;

		if (host_json_parse_int_in(obj, obj_len, "buf_len_bits", &blb))
			return -1;
		v->buf_len_bits = (size_t)blb;
		v->expected_out_len = (v->buf_len_bits + 7) / 8;
	}

	if (host_json_parse_string_in(obj, obj_len, "out", hex, sizeof(hex)))
		return -1;
	{
		size_t out_len = 0;

		if (host_hex_decode(hex, v->expected_out, sizeof(v->expected_out),
				    &out_len))
			return -1;
	}

	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	u8 out[MAX_OUT_LEN];
	int ret;

	memset(out, 0, sizeof(out));
	if (v->fn == FN_SHA256_PRF) {
		ret = sha256_prf(v->key, v->key_len, v->label, v->data,
				 v->data_len, out, v->buf_len);
	} else {
		ret = sha256_prf_bits(v->key, v->key_len, v->label, v->data,
				      v->data_len, out, v->buf_len_bits);
	}

	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
			v->expect_ret, ret);
		return -1;
	}
	if (ret == 0 && memcmp(out, v->expected_out, v->expected_out_len) != 0) {
		size_t j;

		fprintf(stderr, "%s: output mismatch\n", v->name);
		fprintf(stderr, "  expected: ");
		for (j = 0; j < v->expected_out_len; j++)
			fprintf(stderr, "%02x", v->expected_out[j]);
		fprintf(stderr, "\n  got:      ");
		for (j = 0; j < v->expected_out_len; j++)
			fprintf(stderr, "%02x", out[j]);
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "sha256_prf_vectors.json";
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
#ifndef RUST_SHA256_PRF_ORACLE
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
		printf("all %zu sha256-prf vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/sha256-prf.c)\n",
		       executed, skipped);
	else
		printf("all %zu sha256-prf vectors passed "
		       "(oracle: core/crypto/sha256-prf.c)\n",
		       executed);
	return 0;
}
