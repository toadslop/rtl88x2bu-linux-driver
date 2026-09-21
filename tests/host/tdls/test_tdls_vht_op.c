// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_tdls_vht_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char data_hex[64];
	char sta_map_hex[8];
	char expect_mcs_map_hex[8];
	int cur_bwmode, vht_option, op_mode, expect_bw, check_mcs_map;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "data_hex", v->data_hex, sizeof(v->data_hex));
	host_json_parse_string_in(obj, len, "sta_map_hex", v->sta_map_hex, sizeof(v->sta_map_hex));
	host_json_parse_string_in(obj, len, "expect_mcs_map_hex", v->expect_mcs_map_hex,
				  sizeof(v->expect_mcs_map_hex));
#define PI(f) host_json_parse_int_in(obj, len, #f, &v->f)
	PI(cur_bwmode);
	PI(vht_option);
	PI(op_mode);
	PI(expect_bw);
	PI(check_mcs_map);
	return 0;
}

static int maps_equal(const u8 *a, const u8 *b)
{
	return a[0] == b[0] && a[1] == b[1];
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0, i, fail = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 16, parse_vec, &n))
		return 2;
	for (i = 0; i < n; i++) {
		struct vector *v = &vecs[i];
		_adapter adapter;
		struct sta_info sta;
		u8 data[32], op_mode[1], expect_map[2];
		size_t data_len = 0;

		memset(&adapter, 0, sizeof(adapter));
		memset(&sta, 0, sizeof(sta));
		adapter.mlmepriv.vhtpriv.vht_option = (u8)v->vht_option;
		adapter.mlmeextpriv.cur_bwmode = (u8)v->cur_bwmode;
		if (v->sta_map_hex[0]) {
			u8 map[2];
			size_t ml = 0;

			if (host_hex_decode(v->sta_map_hex, map, 2, &ml) || ml != 2)
				return 1;
			memcpy(sta.vhtpriv.vht_mcs_map, map, 2);
		}
		if (v->data_hex[0]) {
			if (host_hex_decode(v->data_hex, data, sizeof(data), &data_len))
				return 1;
			rtw_tdls_process_vht_operation(&adapter, &sta, data, (u8)data_len);
		} else {
			op_mode[0] = (u8)v->op_mode;
			rtw_tdls_process_vht_op_mode_notify(&adapter, &sta, op_mode, 1);
		}
		if (sta.cmn.bw_mode != (u8)v->expect_bw) {
			fprintf(stderr, "FAIL %s bw got %u expect %d\n", v->name,
				sta.cmn.bw_mode, v->expect_bw);
			fail++;
			continue;
		}
		if (v->check_mcs_map && v->expect_mcs_map_hex[0]) {
			size_t eml = 0;

			if (host_hex_decode(v->expect_mcs_map_hex, expect_map, 2, &eml) ||
			    eml != 2) {
				fprintf(stderr, "FAIL %s bad expect_mcs_map_hex\n", v->name);
				return 1;
			}
			if (!maps_equal(sta.vhtpriv.vht_mcs_map, expect_map)) {
				fprintf(stderr,
					"FAIL %s mcs_map got %02x%02x expect %s\n",
					v->name, sta.vhtpriv.vht_mcs_map[0],
					sta.vhtpriv.vht_mcs_map[1], v->expect_mcs_map_hex);
				fail++;
			}
		}
	}
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
