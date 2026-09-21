// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_tdls_vht_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char data_hex[64];
	int data_len, vht_enable, wireless_mode, expect_vht, expect_vht_option;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->data_len = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "data_hex", v->data_hex, sizeof(v->data_hex));
#define PI(f) host_json_parse_int_in(obj, len, #f, &v->f)
	PI(data_len);
	PI(vht_enable);
	PI(wireless_mode);
	PI(expect_vht);
	PI(expect_vht_option);
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
		adapter.registrypriv.vht_enable = (u8)v->vht_enable;
		adapter.registrypriv.wireless_mode = (u8)v->wireless_mode;
		if (host_hex_decode(v->data_hex, data, sizeof(data), &data_len))
			return 1;
		if (v->data_len >= 0)
			data_len = (size_t)v->data_len;
		rtw_tdls_process_vht_cap(&adapter, &sta, data, (u8)data_len);
		if (!!((sta.flags & WLAN_STA_VHT) != 0) != !!v->expect_vht ||
		    !!sta.vhtpriv.vht_option != !!v->expect_vht_option)
			fail++;
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
