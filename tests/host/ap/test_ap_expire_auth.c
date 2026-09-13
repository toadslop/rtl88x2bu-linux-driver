// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_expire_auth_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	u8 expire0, expect0, expect_flush;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "expire0", &tmp))
		return -1;
	v->expire0 = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect0", &tmp))
		return -1;
	v->expect0 = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_flush", &tmp))
		return -1;
	v->expect_flush = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;

	host_expire_auth_adapter_init(&adapter);
	host_expire_auth_add_sta(&adapter, v->expire0);
	rtw_ap_expire_auth_list(&adapter);
	if (adapter.stapriv.sta_pool[0].expire_to != v->expect0 ||
	    host_expire_auth_flush_count() != v->expect_flush) {
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
