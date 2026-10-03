// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_mbo_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 64
#define MAX_IE 256

struct vector {
	char name[MAX_NAME];
	char op[16];
	char ie_hex[512];
	char expect_frame_hex[64];
	char ch_hex[128];
	int limit;
	int expect_found;
	int expect_plen;
	int attr_id;
	int expect_attr_len;
	int expect_attr_val;
	int expect_u32;
	int payload_len;
	int op_class;
	int preference;
	int reason;
	int ielength;
	int expect_u8;
	int query_ch;
};

static _adapter g_adapter;

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "ie_hex", v->ie_hex, sizeof(v->ie_hex));
	host_json_parse_string_in(obj, len, "expect_frame_hex", v->expect_frame_hex,
				  sizeof(v->expect_frame_hex));
	host_json_parse_string_in(obj, len, "ch_hex", v->ch_hex, sizeof(v->ch_hex));
	host_json_parse_int_in(obj, len, "limit", &v->limit);
	host_json_parse_int_in(obj, len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, len, "expect_plen", &v->expect_plen);
	host_json_parse_int_in(obj, len, "attr_id", &v->attr_id);
	host_json_parse_int_in(obj, len, "expect_attr_len", &v->expect_attr_len);
	host_json_parse_int_in(obj, len, "expect_attr_val", &v->expect_attr_val);
	host_json_parse_int_in(obj, len, "expect_u32", &v->expect_u32);
	host_json_parse_int_in(obj, len, "payload_len", &v->payload_len);
	host_json_parse_int_in(obj, len, "op_class", &v->op_class);
	host_json_parse_int_in(obj, len, "preference", &v->preference);
	host_json_parse_int_in(obj, len, "reason", &v->reason);
	host_json_parse_int_in(obj, len, "ielength", &v->ielength);
	host_json_parse_int_in(obj, len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, len, "query_ch", &v->query_ch);
	return 0;
}

static int hex_eq(const u8 *buf, size_t len, const char *hex)
{
	u8 expect[64];
	size_t elen = 0;

	if (host_hex_decode(hex, expect, sizeof(expect), &elen) || elen != len)
		return 1;
	return memcmp(buf, expect, len) != 0;
}

static int run_vec(struct vector *v)
{
	u8 ie[MAX_IE];
	size_t ie_len = 0;
	u32 plen = 0, attr_len = 0;
	u8 *p;

	if (v->ie_hex[0] && host_hex_decode(v->ie_hex, ie, sizeof(ie), &ie_len))
		return 1;

	if (!strcmp(v->op, "ie_get")) {
		p = host_mbo_ie_get(ie, &plen, (u32)v->limit);
		if ((p != NULL) != v->expect_found ||
		    (p && plen != (u32)v->expect_plen))
			return 1;
	} else if (!strcmp(v->op, "attrs_get")) {
		p = host_mbo_attrs_get(ie, (u32)v->limit, (u8)v->attr_id, &attr_len);
		if ((p != NULL) != v->expect_found ||
		    (p && (int)attr_len != v->expect_attr_len) ||
		    (p && *(p + 2) != (u8)v->expect_attr_val))
			return 1;
	} else if (!strcmp(v->op, "attr_sz")) {
		host_mbo_adapter_clear(&g_adapter);
		if (host_mbo_attr_sz_get(&g_adapter, (u8)v->attr_id) !=
		    (u32)v->expect_u32)
			return 1;
	} else if (!strcmp(v->op, "attr_sz_npref")) {
		u8 chs[8];
		size_t ch_len = 0;

		host_mbo_adapter_clear(&g_adapter);
		if (host_hex_decode(v->ch_hex, chs, sizeof(chs), &ch_len))
			return 1;
		host_mbo_seed_npref(&g_adapter, 0, (u8)v->op_class, (u8)ch_len, chs,
				    (u8)v->preference, (u8)v->reason);
		if (host_mbo_attr_sz_get(&g_adapter, RTW_MBO_ATTR_NPREF_CH_RPT_ID) !=
		    (u32)v->expect_u32)
			return 1;
	} else if (!strcmp(v->op, "build_hdr")) {
		u8 frame[32];
		u8 *pframe = frame;
		struct pkt_attrib attrib = { .pktlen = 0 };

		host_mbo_build_mbo_ie_hdr(&pframe, &attrib, (u8)v->payload_len);
		if (hex_eq(frame, (size_t)(pframe - frame), v->expect_frame_hex))
			return 1;
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
