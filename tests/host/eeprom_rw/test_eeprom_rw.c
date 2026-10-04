// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_eeprom_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48], op[24];
	int reg, data, addr_off, sz;
	int expect_u16, expect_ret, expect_writes;
	char hex[320], expect_hex[64];
};

static int nib(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c - 'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c - 'A';
	return -1;
}

static int load_hex(const char *s, u8 *out, int max)
{
	int n = 0;

	while (s && *s && n < max) {
		int hi, lo;

		while (*s == ' ' || *s == ',')
			s++;
	 if (!s[0] || !s[1])
			break;
		hi = nib(s[0]);
		lo = nib(s[1]);
		if (hi < 0 || lo < 0)
			break;
		out[n++] = (u8)((hi << 4) | lo);
		s += 2;
	}
	return n;
}

static int hex_eq(const u8 *got, int n, const char *expect)
{
	u8 exp[32];
	int en = load_hex(expect, exp, (int)sizeof(exp));

	if (en != n)
		return 0;
	return memcmp(got, exp, (size_t)n) == 0;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->expect_u16 = v->expect_ret = v->expect_writes = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "reg", &v->reg);
	host_json_parse_int_in(obj, len, "data", &v->data);
	host_json_parse_int_in(obj, len, "addr_off", &v->addr_off);
	host_json_parse_int_in(obj, len, "sz", &v->sz);
	if (host_json_parse_int_in(obj, len, "expect_u16", &v->expect_u16))
		v->expect_u16 = -1;
	if (host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret))
		v->expect_ret = -1;
	host_json_parse_int_in(obj, len, "expect_writes", &v->expect_writes);
	host_json_parse_string_in(obj, len, "hex", v->hex, sizeof(v->hex));
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex,
				  sizeof(v->expect_hex));
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_eeprom_adapter *ad = host_eeprom_adapter();
	u16 out;
	u8 buf[16], ret;

	if (!strcmp(v->op, "reset"))
		host_eeprom_reset();
	else if (!strcmp(v->op, "load_hex")) {
		u8 seq[128];
		int n = load_hex(v->hex, seq, (int)sizeof(seq));

		host_eeprom_set_read_sequence(seq, n);
	} else if (!strcmp(v->op, "surprise"))
		host_eeprom_set_surprise(1);
	else if (!strcmp(v->op, "read16")) {
		out = eeprom_read16(ad, (u16)v->reg);
		if (v->expect_u16 >= 0 && out != (u16)v->expect_u16)
			goto bad;
	} else if (!strcmp(v->op, "write16"))
		eeprom_write16(ad, (u16)v->reg, (u16)v->data);
	else if (!strcmp(v->op, "read_sz")) {
		memset(buf, 0, sizeof(buf));
		eeprom_read_sz(ad, (u16)v->reg, buf, (u32)v->sz);
		if (v->expect_hex[0] && !hex_eq(buf, v->sz, v->expect_hex))
			goto bad;
	} else if (!strcmp(v->op, "read")) {
		memset(buf, 0, sizeof(buf));
		ret = eeprom_read(ad, (u32)v->addr_off, (u8)v->sz, buf);
		if (v->expect_ret >= 0 && ret != (u8)v->expect_ret)
			goto bad;
		if (v->expect_hex[0] && !hex_eq(buf, v->sz, v->expect_hex))
			goto bad;
	} else if (!strcmp(v->op, "read_content"))
		read_eeprom_content(ad);
	else
		goto bad;

	if (v->expect_writes >= 0 &&
	    host_eeprom_write_count() != v->expect_writes)
		goto bad;
	printf("PASS %s\n", v->name);
	return 0;
bad:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[32];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "eeprom_rw_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 32, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad |= run_vec(&vecs[i]);
	return bad ? 1 : 0;
}
