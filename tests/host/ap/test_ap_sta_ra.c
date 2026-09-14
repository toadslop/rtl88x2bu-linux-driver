// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_sta_ra_types.h"
#include "host_vector_json.h"

static u64 host_hal_ramask;
static u8 host_hal_ra_called, host_hal_wset_called;

void host_sta_ra_reset(void)
{
	host_hal_ramask = 0;
	host_hal_ra_called = 0;
	host_hal_wset_called = 0;
}

void host_sta_ra_set_hal_ramask(u64 ramask)
{
	host_hal_ramask = ramask;
}

u8 host_sta_ra_hal_ra_called(void)
{
	return host_hal_ra_called;
}

u8 host_sta_ra_hal_wset_called(void)
{
	return host_hal_wset_called;
}

void rtw_hal_update_sta_ra_info(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	host_hal_ra_called = 1;
	psta->cmn.ra_info.ramask = host_hal_ramask;
}

void rtw_hal_update_sta_wset(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
	host_hal_wset_called = 1;
}

struct vector {
	char name[64];
	u32 ds_config, state;
	u64 hal_ramask;
	u8 vht_option, expect_mode, expect_hal_ra, expect_hal_wset;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ds_config", &tmp))
		return -1;
	v->ds_config = (u32)tmp;
	if (host_json_parse_int_in(obj, len, "state", &tmp))
		return -1;
	v->state = (u32)tmp;
	if (host_json_parse_int_in(obj, len, "hal_ramask", &tmp))
		return -1;
	v->hal_ramask = (u64)(u32)tmp;
	if (host_json_parse_int_in(obj, len, "vht_option", &tmp))
		return -1;
	v->vht_option = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_mode", &tmp))
		return -1;
	v->expect_mode = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_hal_ra", &tmp))
		return -1;
	v->expect_hal_ra = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_hal_wset", &tmp))
		return -1;
	v->expect_hal_wset = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;

	host_sta_ra_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.mlmepriv.cur_network.network.Configuration.DSConfig = v->ds_config;
	sta.state = v->state;
	sta.vhtpriv.vht_option = v->vht_option;
	host_sta_ra_set_hal_ramask(v->hal_ramask);

	rtw_ap_update_sta_ra_info(&adapter, &sta);

	if (host_sta_ra_hal_ra_called() != v->expect_hal_ra) {
		fprintf(stderr, "FAIL %s hal_ra_called\n", v->name);
		return -1;
	}
	if (host_sta_ra_hal_wset_called() != v->expect_hal_wset) {
		fprintf(stderr, "FAIL %s hal_wset_called\n", v->name);
		return -1;
	}
	if (sta.wireless_mode != v->expect_mode) {
		fprintf(stderr, "FAIL %s mode=%u expect=%u\n", v->name,
			sta.wireless_mode, v->expect_mode);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector v[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], v, sizeof(v[0]), 16, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&v[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
