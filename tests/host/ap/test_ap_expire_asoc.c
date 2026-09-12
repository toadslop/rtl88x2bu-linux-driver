// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_expire_asoc_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u8 sta_alive, sta_expire_to, ap_expire_to;
	u8 expect_expire_to, expect_keepalive_trycnt;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "sta_alive", &tmp))
		return -1;
	v->sta_alive = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "sta_expire_to", &tmp))
		return -1;
	v->sta_expire_to = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "ap_expire_to", &tmp))
		return -1;
	v->ap_expire_to = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_expire_to", &tmp))
		return -1;
	v->expect_expire_to = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_keepalive_trycnt", &tmp))
		return -1;
	v->expect_keepalive_trycnt = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;

	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.stapriv.expire_to = v->ap_expire_to;
	sta.expire_to = v->sta_expire_to;
	sta.keep_alive_trycnt = 7;
	expire_timeout_asoc_step(&adapter, &sta, v->sta_alive);
	if (sta.expire_to != v->expect_expire_to ||
	    sta.keep_alive_trycnt != v->expect_keepalive_trycnt) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[8];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 8, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
