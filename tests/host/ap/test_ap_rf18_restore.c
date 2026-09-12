// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_rf18_restore_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int reg0, reg1, union_ok;
	int mlme_ch, mlme_offset, mlme_bw;
	int union_ch, union_bw, union_offset;
	int expect_set, expect_ch, expect_offset, expect_bw;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "reg0", &tmp))
		return -1;
	v->reg0 = tmp;
	if (host_json_parse_int_in(obj, len, "reg1", &tmp))
		return -1;
	v->reg1 = tmp;
	if (host_json_parse_int_in(obj, len, "union_ok", &tmp))
		return -1;
	v->union_ok = tmp;
	if (host_json_parse_int_in(obj, len, "mlme_ch", &tmp))
		return -1;
	v->mlme_ch = tmp;
	if (host_json_parse_int_in(obj, len, "mlme_offset", &tmp))
		return -1;
	v->mlme_offset = tmp;
	if (host_json_parse_int_in(obj, len, "mlme_bw", &tmp))
		return -1;
	v->mlme_bw = tmp;
	if (host_json_parse_int_in(obj, len, "union_ch", &tmp))
		return -1;
	v->union_ch = tmp;
	if (host_json_parse_int_in(obj, len, "union_bw", &tmp))
		return -1;
	v->union_bw = tmp;
	if (host_json_parse_int_in(obj, len, "union_offset", &tmp))
		return -1;
	v->union_offset = tmp;
	if (host_json_parse_int_in(obj, len, "expect_set", &tmp))
		return -1;
	v->expect_set = tmp;
	if (host_json_parse_int_in(obj, len, "expect_ch", &tmp))
		return -1;
	v->expect_ch = tmp;
	if (host_json_parse_int_in(obj, len, "expect_offset", &tmp))
		return -1;
	v->expect_offset = tmp;
	if (host_json_parse_int_in(obj, len, "expect_bw", &tmp))
		return -1;
	v->expect_bw = tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	u8 ch, offset, bw;

	host_rf18_reset();
	memset(&adapter, 0, sizeof(adapter));
	adapter.mlmeextpriv.cur_channel = (u8)v->mlme_ch;
	adapter.mlmeextpriv.cur_ch_offset = (u8)v->mlme_offset;
	adapter.mlmeextpriv.cur_bwmode = (u8)v->mlme_bw;
	host_rf18_set_reg(0, (u32)v->reg0);
	host_rf18_set_reg(1, (u32)v->reg1);
	host_rf18_set_union_ok((u8)v->union_ok, (u8)v->union_ch, (u8)v->union_bw,
			       (u8)v->union_offset);
	rtw_check_restore_rf18(&adapter);
	if (host_rf18_set_channel_called() != (u8)v->expect_set) {
		fprintf(stderr, "FAIL %s set_called\n", v->name);
		return -1;
	}
	if (!v->expect_set)
		return 0;
	host_rf18_last_set_channel(&ch, &offset, &bw);
	if (ch != (u8)v->expect_ch || offset != (u8)v->expect_offset ||
	    bw != (u8)v->expect_bw || adapter.hal_data.current_channel != 0) {
		fprintf(stderr, "FAIL %s ch=%u/%u off=%u/%u bw=%u/%u hal=%u\n", v->name,
			ch, v->expect_ch, offset, v->expect_offset, bw, v->expect_bw,
			adapter.hal_data.current_channel);
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
