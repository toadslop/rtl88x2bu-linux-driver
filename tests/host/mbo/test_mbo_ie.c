// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mbo_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64
#define MAX_IE 256

struct vector {
	char name[MAX_NAME];
	char op[16];
	char ie_hex[512];
	int limit;
	int expect_found;
	int expect_plen;
	int attr_id;
	int expect_attr_len;
	int expect_attr_val;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "ie_hex", v->ie_hex, sizeof(v->ie_hex));
	host_json_parse_int_in(obj, len, "limit", &v->limit);
	host_json_parse_int_in(obj, len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, len, "expect_plen", &v->expect_plen);
	host_json_parse_int_in(obj, len, "attr_id", &v->attr_id);
	host_json_parse_int_in(obj, len, "expect_attr_len", &v->expect_attr_len);
	host_json_parse_int_in(obj, len, "expect_attr_val", &v->expect_attr_val);
	return 0;
}

static int run_vec(struct vector *v)
{
	u8 ie[MAX_IE];
	size_t ie_len = 0;
	u32 plen = 0, attr_len = 0;
	u8 *p;

	if (host_hex_decode(v->ie_hex, ie, sizeof(ie), &ie_len))
		return 1;

	if (!strcmp(v->op, "ie_get")) {
		p = host_mbo_ie_get(ie, &plen, (u32)v->limit);
		if ((p != NULL) != v->expect_found ||
		    (p && plen != (u32)v->expect_plen)) {
			fprintf(stderr, "FAIL %s ie_get\n", v->name);
			return 1;
		}
	} else if (!strcmp(v->op, "attrs_get")) {
		p = host_mbo_attrs_get(ie, (u32)v->limit, (u8)v->attr_id, &attr_len);
		if ((p != NULL) != v->expect_found ||
		    (p && (int)attr_len != v->expect_attr_len) ||
		    (p && *(p + 2) != (u8)v->expect_attr_val)) {
			fprintf(stderr, "FAIL %s attrs_get\n", v->name);
			return 1;
		}
	} else {
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t n = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "mbo_ie_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS, parse_vec, &n))
		return 2;
	for (size_t i = 0; i < n; i++)
		bad += run_vec(&vecs[i]);
	if (!bad)
		printf("PASS %zu vectors (%s)\n", n, path);
	return bad ? 1 : 0;
}
