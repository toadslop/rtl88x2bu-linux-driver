// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_aka_chk_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int ap_mode, sta_sleep, nulldata_ret;
	int expect_ret, expect_ps, expect_nulldata;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ap_mode", &tmp))
		return -1;
	v->ap_mode = tmp;
	if (host_json_parse_int_in(obj, len, "sta_sleep", &tmp))
		return -1;
	v->sta_sleep = tmp;
	if (host_json_parse_int_in(obj, len, "nulldata_ret", &tmp))
		return -1;
	v->nulldata_ret = tmp;
	if (host_json_parse_int_in(obj, len, "expect_ret", &tmp))
		return -1;
	v->expect_ret = tmp;
	if (host_json_parse_int_in(obj, len, "expect_ps", &tmp))
		return -1;
	v->expect_ps = tmp;
	if (host_json_parse_int_in(obj, len, "expect_nulldata", &tmp))
		return -1;
	v->expect_nulldata = tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;
	int ret;

	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	if (v->ap_mode)
		adapter.mlmepriv.fwstate = WIFI_AP_STATE;
	if (v->sta_sleep)
		sta.state = WIFI_SLEEP_STATE;
	host_aka_chk_reset();
	host_aka_chk_set_nulldata_ret(v->nulldata_ret);
	ret = issue_aka_chk_frame(&adapter, &sta);
	if (ret != v->expect_ret ||
	    host_aka_chk_nulldata_called() != (u8)v->expect_nulldata ||
	    (v->expect_nulldata && host_aka_chk_last_nulldata_ps() != v->expect_ps)) {
		fprintf(stderr, "FAIL %s ret=%d ps=%d called=%u\n", v->name, ret,
			host_aka_chk_last_nulldata_ps(), host_aka_chk_nulldata_called());
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
