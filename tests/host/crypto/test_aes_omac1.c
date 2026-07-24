// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for aes-omac1.c (W2-01).
 *
 * oracle: core/crypto/aes-omac1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"
#include "aes_wrap.h"

#define MAX_VECTORS 32
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
	int rust_only;
	int num_elem_override;
	int has_num_elem_override;
	int null_mac;
};

static int json_parse_fn_dispatch(const char *obj, size_t obj_len,
				  const char *key, enum omac1_fn *out)
{
	char buf[64];

	if (host_json_parse_string_in(obj, obj_len, key, buf, sizeof(buf)))
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
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t count = 0;

	if (!p || p >= obj + obj_len || *p != '[')
		return -1;
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
	return count ? 0 : -1;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	int key_len = 0;
	size_t decoded_key_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (json_parse_fn_dispatch(obj, obj_len, "fn", &v->fn))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;
	{
		int override = 0;

		if (host_json_parse_int_in(obj, obj_len, "num_elem_override",
					   &override) == 0) {
			v->num_elem_override = override;
			v->has_num_elem_override = 1;
		}
	}
	if (host_json_parse_bool_in(obj, obj_len, "null_mac", &v->null_mac) != 0)
		v->null_mac = 0;
	if (host_json_parse_int_in(obj, obj_len, "key_len", &key_len))
		return -1;
	v->key_len = (size_t)key_len;
	if (host_json_parse_string_in(obj, obj_len, "key", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->key, sizeof(v->key), &decoded_key_len))
		return -1;
	if (decoded_key_len != v->key_len)
		return -1;

	if (host_json_parse_string_in(obj, obj_len, "data", hex, sizeof(hex)) == 0) {
		if (host_hex_decode(hex, v->elem[0], sizeof(v->elem[0]), &v->elem_len[0]))
			return -1;
		v->num_elem = 1;
	} else if (json_parse_elements_in(obj, obj_len, "elements", v)) {
		return -1;
	}

	if (host_json_parse_string_in(obj, obj_len, "mac", hex, sizeof(hex)))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret))
		return -1;
	{
		size_t mac_len = 0;

		if (host_hex_decode(hex, v->expected_mac, sizeof(v->expected_mac),
				    &mac_len))
			return -1;
		if (v->expect_ret == 0 && mac_len != 16)
			return -1;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	u8 mac[16];
	const u8 *addr[MAX_ELEMENTS];
	int ret;
	size_t i;
	size_t call_num_elem = v->num_elem;

	if (v->has_num_elem_override)
		call_num_elem = (size_t)v->num_elem_override;

	for (i = 0; i < v->num_elem; i++)
		addr[i] = v->elem[i];

	switch (v->fn) {
	case FN_OMAC1_AES_VECTOR:
		ret = omac1_aes_vector(v->key, v->key_len, call_num_elem, addr,
				       v->elem_len, mac);
		break;
	case FN_OMAC1_AES_128_VECTOR:
		if (v->key_len != 16)
			return -1;
		ret = omac1_aes_128_vector(v->key, call_num_elem, addr,
					     v->elem_len, mac);
		break;
	case FN_OMAC1_AES_128:
		if (v->key_len != 16 || v->num_elem != 1)
			return -1;
		ret = omac1_aes_128(v->key, v->elem[0], v->elem_len[0],
				    v->null_mac ? NULL : mac);
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
#ifndef RUST_OMAC1_ORACLE
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
		printf("all %zu aes-omac1 vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/aes-omac1.c)\n",
		       executed, skipped);
	else
		printf("all %zu aes-omac1 vectors passed "
		       "(oracle: core/crypto/aes-omac1.c)\n",
		       executed);
	return 0;
}
