// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_eeprom_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48], op[24];
	int reg, data;
	int expect_u16, expect_writes;
	char hex[320];
};

static int load_hex(const char *s, u8 *out, int max)
{
	int n = 0;

	while (s && *s && n < max) {
		int hi, lo;

		while (*s == ' ' || *s == ',')
			s++;
		if (!s[0] || !s[1])
			break;
		hi = (*s >= 'a') ? 10 + *s - 'a' : (*s >= 'A' ? 10 + *s - 'A' : *s - '0');
		s++;
		lo = (*s >= 'a') ? 10 + *s - 'a' : (*s >= 'A' ? 10 + *s - 'A' : *s - '0');
		s++;
		out[n++] = (u8)((hi << 4) | lo);
	}
	return n;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->expect_u16 = v->expect_writes = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "reg", &v->reg);
	host_json_parse_int_in(obj, len, "data", &v->data);
	if (host_json_parse_int_in(obj, len, "expect_u16", &v->expect_u16))
		v->expect_u16 = -1;
	host_json_parse_int_in(obj, len, "expect_writes", &v->expect_writes);
	host_json_parse_string_in(obj, len, "hex", v->hex, sizeof(v->hex));
	return 0;
}

static int run_vec(struct vector *v)
{
	struct host_eeprom_adapter *ad = host_eeprom_adapter();
	u16 out;

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
	else if (!strcmp(v->op, "read_content"))
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
	struct vector vecs[24];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "eeprom_rw_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 24, parse_vec, &n))
		return 1;
	for (size_t i = 0; i < n; i++)
		bad |= run_vec(&vecs[i]);
	return bad ? 1 : 0;
}
