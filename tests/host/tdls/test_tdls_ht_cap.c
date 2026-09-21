// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_tdls_ht_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char data_hex[128];
	int ht_enable, wireless_mode, ampdu_enable, ap_sgi_20, cur_bwmode, cur_ch_offset;
	int expect_flags_ht, expect_ht_option, expect_ampdu, expect_sgi_20, expect_bw;
	int ap_ampdu_para;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "data_hex", v->data_hex, sizeof(v->data_hex));
#define PI(f) host_json_parse_int_in(obj, len, #f, &v->f)
	PI(ht_enable);
	PI(wireless_mode);
	PI(ampdu_enable);
	PI(ap_ampdu_para);
	PI(ap_sgi_20);
	PI(cur_bwmode);
	PI(cur_ch_offset);
	PI(expect_flags_ht);
	PI(expect_ht_option);
	PI(expect_ampdu);
	PI(expect_sgi_20);
	PI(expect_bw);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 8, parse_vec, &n))
		return 2;
	for (i = 0; i < n; i++) {
		struct vector *v = &vecs[i];
		_adapter adapter;
		struct sta_info sta;
		u8 data[32];
		size_t data_len = 0;

		memset(&adapter, 0, sizeof(adapter));
		memset(&sta, 0, sizeof(sta));
		adapter.registrypriv.ht_enable = (u8)v->ht_enable;
		adapter.registrypriv.wireless_mode = (u8)v->wireless_mode;
		adapter.registrypriv.ampu_enable = (u8)v->ampdu_enable;
		adapter.mlmepriv.htpriv.sgi_20m = (u8)v->ap_sgi_20;
		adapter.mlmeextpriv.cur_bwmode = (u8)v->cur_bwmode;
		adapter.mlmeextpriv.cur_ch_offset = (u8)v->cur_ch_offset;
		adapter.mlmeextpriv.mlmext_info.ap_ampdu_para = (u8)v->ap_ampdu_para;
		if (host_hex_decode(v->data_hex, data, sizeof(data), &data_len))
			return 1;
		rtw_tdls_process_ht_cap(&adapter, &sta, data, (u8)data_len);
		if (!!((sta.flags & WLAN_STA_HT) != 0) != !!v->expect_flags_ht ||
		    !!sta.htpriv.ht_option != !!v->expect_ht_option ||
		    !!sta.htpriv.ampdu_enable != !!v->expect_ampdu ||
		    !!sta.htpriv.sgi_20m != !!v->expect_sgi_20 ||
		    sta.bw_mode != (u8)v->expect_bw) {
			fprintf(stderr, "FAIL %s\n", v->name);
			fail++;
		}
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
