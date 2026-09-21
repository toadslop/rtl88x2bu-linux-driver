// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rson_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[24];
	char hex_str[48];
	char expect_hex[40];
	int expect_int;
};

static struct dvobj_priv g_dvobj;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "hex_str", v->hex_str, sizeof(v->hex_str));
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex,
				  sizeof(v->expect_hex));
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	if (!strcmp(v->op, "init")) {
		init_rtw_rson_data(&g_dvobj);
		if (g_dvobj.rson_data.ver != RTW_RSON_VER ||
		    g_dvobj.rson_data.id != CONFIG_RTW_REPEATER_SON_ID ||
		    g_dvobj.rson_data.hopcnt != RTW_RSON_HC_NOTREADY ||
		    g_dvobj.rson_data.connectible != RTW_RSON_DENYCONNECT)
			return 1;
		for (size_t i = 0; i < sizeof(g_dvobj.rson_data.res); i++) {
			if (g_dvobj.rson_data.res[i] != 0xAA)
				return 1;
		}
		return 0;
	}
	if (!strcmp(v->op, "str2hex")) {
		u8 buf[16], expect[16];
		size_t en = 0;

		if (str2hexbuf(v->hex_str, buf, 16) != v->expect_int)
			return 1;
		if (v->expect_int == _TRUE && v->expect_hex[0]) {
			if (host_hex_decode(v->expect_hex, expect, sizeof(expect), &en))
				return 1;
			if (memcmp(buf, expect, en))
				return 1;
		}
		return 0;
	}
	if (!strcmp(v->op, "verify_ie")) {
		u8 ie[32];
		size_t n = 0;

		if (host_hex_decode(v->hex_str, ie, sizeof(ie), &n))
			return 1;
		return rtw_rson_varify_ie(ie) != (u8)v->expect_int;
	}
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t count = 0, i;
	int fail = 0;

	if (argc < 2)
		return 2;
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 16, parse_vec, &count))
		return 2;
	for (i = 0; i < count; i++)
		fail += run_vec(&vecs[i]);
	printf("%zu/%zu rson core vectors PASS\n", count - (size_t)fail, count);
	return fail ? 1 : 0;
}
