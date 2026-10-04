// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_eeprom_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48], op[16];
	int init_x, init_reg, expect_u16, expect_reg, expect_writes;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->expect_reg = v->expect_writes = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "init_x", &v->init_x);
	host_json_parse_int_in(obj, len, "init_reg", &v->init_reg);
	if (host_json_parse_int_in(obj, len, "expect_u16", &v->expect_u16))
		v->expect_u16 = -1;
	host_json_parse_int_in(obj, len, "expect_reg", &v->expect_reg);
	host_json_parse_int_in(obj, len, "expect_writes", &v->expect_writes);
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_eeprom_adapter *ad = host_eeprom_adapter();
	u16 x;

	if (!strcmp(v->op, "reset"))
		host_eeprom_reset();
	else if (!strcmp(v->op, "set_reg"))
		host_eeprom_set_reg((u8)v->init_reg);
	else if (!strcmp(v->op, "up_clk")) {
		x = (u16)v->init_x;
		up_clk(ad, &x);
		if (v->expect_u16 >= 0 && x != (u16)v->expect_u16)
			goto bad;
	} else if (!strcmp(v->op, "down_clk")) {
		x = (u16)v->init_x;
		down_clk(ad, &x);
		if (v->expect_u16 >= 0 && x != (u16)v->expect_u16)
			goto bad;
	} else if (!strcmp(v->op, "standby"))
		standby(ad);
	else
		goto bad;

	if (v->expect_reg >= 0 && host_eeprom_reg() != (u8)v->expect_reg)
		goto bad;
	if (v->expect_writes >= 0 && host_eeprom_write_count() != v->expect_writes)
		goto bad;
	printf("PASS %s\n", v->name);
	return 0;
bad:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[16];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "eeprom_bitbang_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 16, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad |= run_vec(&vecs[i]);
	return bad ? 1 : 0;
}
