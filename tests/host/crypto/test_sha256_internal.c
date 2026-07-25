// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for sha256-internal.c (W2-05a).
 *
 * oracle: core/crypto/sha256-internal.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_types.h"
#include "host_vector_json.h"

#define SHA256_MAC_LEN 32
#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_ELEMENTS 8
#define MAX_ELEM_LEN 1024

int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);

struct vector {
	char name[MAX_NAME];
	size_t num_elem;
	u8 elem[MAX_ELEMENTS][MAX_ELEM_LEN];
	size_t elem_len[MAX_ELEMENTS];
	u8 expected_mac[SHA256_MAC_LEN];
	int expect_ret;
	int rust_only;
};

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
		if (*hex) {
			if (host_hex_decode(hex, v->elem[count], sizeof(v->elem[count]),
					    &v->elem_len[count]))
				return -1;
		} else {
			v->elem_len[count] = 0;
		}
		count++;
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	v->num_elem = count;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_bool_in(obj, obj_len, "rust_only", &v->rust_only) != 0)
		v->rust_only = 0;

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
		if (v->expect_ret == 0 && mac_len != SHA256_MAC_LEN)
			return -1;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	u8 mac[SHA256_MAC_LEN];
	const u8 *addr[MAX_ELEMENTS];
	int ret;
	size_t i;

	for (i = 0; i < v->num_elem; i++)
		addr[i] = v->elem[i];

	ret = sha256_vector(v->num_elem, addr, v->elem_len, mac);

	if (ret != v->expect_ret) {
		fprintf(stderr, "%s: expected ret %d, got %d\n", v->name,
			v->expect_ret, ret);
		return -1;
	}
	if (ret == 0 && memcmp(mac, v->expected_mac, SHA256_MAC_LEN) != 0) {
		size_t j;

		fprintf(stderr, "%s: mac mismatch\n", v->name);
		fprintf(stderr, "  expected: ");
		for (j = 0; j < SHA256_MAC_LEN; j++)
			fprintf(stderr, "%02x", v->expected_mac[j]);
		fprintf(stderr, "\n  got:      ");
		for (j = 0; j < SHA256_MAC_LEN; j++)
			fprintf(stderr, "%02x", mac[j]);
		fprintf(stderr, "\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "sha256_internal_vectors.json";
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
#ifndef RUST_SHA256_INTERNAL_ORACLE
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
		printf("all %zu sha256-internal vectors passed (%zu rust-only skipped; "
		       "oracle: core/crypto/sha256-internal.c)\n",
		       executed, skipped);
	else
		printf("all %zu sha256-internal vectors passed "
		       "(oracle: core/crypto/sha256-internal.c)\n",
		       executed);
	return 0;
}
