// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_sta_info_types.h"
#include "host_vector_json.h"

void rtw_hal_set_hwreg(_adapter *padapter, enum hw_var variable, u8 *val)
{
	(void)padapter;
	(void)variable;
	(void)val;
}

struct vector {
	char name[64];
	u8 ap_bf_cap;
	u32 sta_tx_bf;
	u8 expect_sta_bf;
	u8 expect_ht_beamform;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "ap_bf_cap", &tmp))
		return -1;
	v->ap_bf_cap = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "sta_tx_bf", &tmp))
		return -1;
	v->sta_tx_bf = (u32)tmp;
	if (host_json_parse_int_in(obj, len, "expect_sta_bf", &tmp))
		return -1;
	v->expect_sta_bf = (u8)tmp;
	if (host_json_parse_int_in(obj, len, "expect_ht_beamform", &tmp))
		return -1;
	v->expect_ht_beamform = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	struct sta_info sta;

	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));
	adapter.mlmepriv.htpriv.beamform_cap = v->ap_bf_cap;
	sta.htpriv.ht_cap.tx_BF_cap_info = v->sta_tx_bf;
	update_sta_info_apmode_ht_bf_cap(&adapter, &sta);
	if (sta.htpriv.beamform_cap != v->expect_sta_bf ||
	    sta.cmn.bf_info.ht_beamform_cap != v->expect_ht_beamform) {
		fprintf(stderr,
			"FAIL %s: got sta_bf=0x%02x ht_bf=0x%02x expect 0x%02x 0x%02x\n",
			v->name, sta.htpriv.beamform_cap, sta.cmn.bf_info.ht_beamform_cap,
			v->expect_sta_bf, v->expect_ht_beamform);
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
