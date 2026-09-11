// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_sta_mgt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	u8 mac[ETH_ALEN];
	u8 drain_free;
	u8 repeat;
	int expect_null;
	int expect_null_on_last;
	int expect_asoc_count;
	int expect_free_count;
	int expect_lookup;
};

static int parse_mac_opt(const char *obj, size_t len, const char *key, u8 *out)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t n = 0;

	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	return host_hex_decode(hex, out, ETH_ALEN, &n) || n != ETH_ALEN;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	v->expect_asoc_count = -1;
	v->expect_free_count = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_int_in(obj, len, "drain_free", (int *)&v->drain_free);
	host_json_parse_int_in(obj, len, "repeat", (int *)&v->repeat);
	host_json_parse_int_in(obj, len, "expect_null", &v->expect_null);
	host_json_parse_int_in(obj, len, "expect_null_on_last", &v->expect_null_on_last);
	host_json_parse_int_in(obj, len, "expect_asoc_count", &v->expect_asoc_count);
	host_json_parse_int_in(obj, len, "expect_free_count", &v->expect_free_count);
	host_json_parse_int_in(obj, len, "expect_lookup", &v->expect_lookup);
	if (!v->repeat)
		v->repeat = 1;
	return parse_mac_opt(obj, len, "mac", v->mac);
}

static int run_vector(const struct vector *v)
{
	_adapter a;
	struct sta_info *sta, *got;
	u8 i;

	host_sta_mgt_alloc_setup(&a);
	if (v->drain_free && host_sta_mgt_alloc_drain(&a, v->drain_free))
		return -1;

	for (i = 0; i < v->repeat; i++) {
		sta = rtw_alloc_stainfo(&a.stapriv, v->mac); /* oracle: C or Rust */
		if (v->expect_null_on_last && i + 1 == v->repeat) {
			if (sta)
				return -1;
			continue;
		}
		if (v->expect_null && sta)
			return -1;
		if (!v->expect_null && !sta)
			return -1;
	}

	if (v->expect_asoc_count >= 0 &&
	    a.stapriv.asoc_sta_count != v->expect_asoc_count)
		return -1;
	if (v->expect_free_count >= 0 &&
	    host_sta_mgt_alloc_free_count(&a) != v->expect_free_count)
		return -1;
	if (v->expect_lookup) {
		got = rtw_get_stainfo(&a.stapriv, v->mac);
		if (!got || _rtw_memcmp(got->cmn.mac_addr, v->mac, ETH_ALEN) != _TRUE)
			return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[MAX_VECTORS];
	size_t n = 0, i, fail = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], v, sizeof(v[0]), MAX_VECTORS, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i])) {
			fprintf(stderr, "FAIL: %s\n", v[i].name);
			fail++;
		}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
