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
	char fn[MAX_NAME];
	int expect_ret;
	int expect_free_count;
	int expect_asoc_count;
};

extern u32 _rtw_init_sta_priv(struct sta_priv *pstapriv);
extern u32 _rtw_free_sta_priv(struct sta_priv *pstapriv);
extern void rtw_mfree_stainfo(struct sta_info *psta);
extern u32 rtw_init_bcmc_stainfo(_adapter *padapter);

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	v->expect_free_count = -1;
	v->expect_asoc_count = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, len, "expect_free_count", &v->expect_free_count);
	host_json_parse_int_in(obj, len, "expect_asoc_count", &v->expect_asoc_count);
	return 0;
}

static int run_mfree_vector(void)
{
	_adapter a;
	struct sta_info sta;

	host_sta_mgt_free_setup(&a);
	memset(&sta, 0, sizeof(sta));
	_rtw_init_stainfo(&sta);
	rtw_mfree_stainfo(&sta);
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter a;
	u32 ret;

	host_sta_mgt_free_setup(&a);

	if (!strcmp(v->fn, "_rtw_init_sta_priv")) {
		ret = _rtw_init_sta_priv(&a.stapriv);
		if ((int)ret != v->expect_ret)
			return -1;
		if (v->expect_free_count >= 0 &&
		    host_sta_mgt_alloc_free_count(&a) != v->expect_free_count)
			return -1;
		return 0;
	}

	if (!strcmp(v->fn, "init_free_sta_priv")) {
		ret = _rtw_init_sta_priv(&a.stapriv);
		if ((int)ret != v->expect_ret)
			return -1;
		ret = _rtw_free_sta_priv(&a.stapriv);
		if ((int)ret != v->expect_ret)
			return -1;
		return 0;
	}

	if (!strcmp(v->fn, "rtw_mfree_stainfo"))
		return run_mfree_vector();

	if (!strcmp(v->fn, "rtw_init_bcmc_stainfo")) {
		ret = _rtw_init_sta_priv(&a.stapriv);
		if ((int)ret != _SUCCESS)
			return -1;
		ret = rtw_init_bcmc_stainfo(&a);
		if ((int)ret != v->expect_ret)
			return -1;
		if (v->expect_asoc_count >= 0 &&
		    a.stapriv.asoc_sta_count != v->expect_asoc_count)
			return -1;
		return 0;
	}

	fprintf(stderr, "unknown fn: %s\n", v->fn);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvectors = 0;
	size_t i;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}

	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvectors))
		return 2;

	for (i = 0; i < nvectors; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			return 1;
		}
	}

	printf("OK: %zu vectors\n", nvectors);
	return 0;
}
