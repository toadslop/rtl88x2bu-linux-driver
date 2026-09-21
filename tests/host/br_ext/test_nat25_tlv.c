// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_br_ext_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[16];
	char mac_hex[24];
	char data_hex[220];
	char expect_hex[220];
	int len, tag, len8b, src_off, pull_len, expect_int;
};

unsigned char *host_scan_tlv(u8 *data, int len, u8 tag, u8 len8b);
int host_update_nd_link_layer_addr(u8 *data, int len, u8 *replace_mac);
void host_convert_ipv6_mac_to_mc(struct host_sk_buff *skb);
int host_skb_pull_and_merge(struct host_sk_buff *skb, u8 *src, int len);

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->len = v->tag = v->len8b = v->src_off = v->pull_len = -1;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "mac_hex", v->mac_hex, sizeof(v->mac_hex));
	host_json_parse_string_in(obj, len, "data_hex", v->data_hex, sizeof(v->data_hex));
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex,
				  sizeof(v->expect_hex));
	host_json_parse_int_in(obj, len, "len", &v->len);
	host_json_parse_int_in(obj, len, "tag", &v->tag);
	host_json_parse_int_in(obj, len, "len8b", &v->len8b);
	host_json_parse_int_in(obj, len, "src_off", &v->src_off);
	host_json_parse_int_in(obj, len, "pull_len", &v->pull_len);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_one(struct vector *v)
{
	u8 buf[128], expect[128], mac[6];
	size_t n = 0, data_len = 0;
	int got;

	if (!strcmp(v->op, "scan_tlv")) {
		unsigned char *p;

		host_hex_decode(v->data_hex, buf, sizeof(buf), &n);
		p = host_scan_tlv(buf, v->len, (u8)v->tag, (u8)v->len8b);
		got = p ? (int)(p - buf) : -1;
		if (got != v->expect_int)
			goto fail;
	} else if (!strcmp(v->op, "update_nd")) {
		host_hex_decode(v->data_hex, buf, sizeof(buf), &data_len);
		host_hex_decode(v->mac_hex, mac, 6, &n);
		got = host_update_nd_link_layer_addr(buf, (int)data_len, mac);
		if (got != v->expect_int)
			goto fail;
		host_hex_decode(v->expect_hex, expect, sizeof(expect), &n);
		if (memcmp(buf, expect, n))
			goto fail;
	} else if (!strcmp(v->op, "convert_mc")) {
		struct host_sk_buff skb;

		host_hex_decode(v->data_hex, buf, sizeof(buf), &data_len);
		skb.data = buf;
		skb.len = (int)data_len;
		host_convert_ipv6_mac_to_mc(&skb);
		host_hex_decode(v->expect_hex, expect, sizeof(expect), &n);
		if (memcmp(buf, expect, n))
			goto fail;
	} else if (!strcmp(v->op, "skb_pull")) {
		struct host_sk_buff skb;
		u8 *src;

		host_hex_decode(v->data_hex, buf, sizeof(buf), &data_len);
		skb.data = buf;
		skb.len = (int)data_len;
		src = buf + v->src_off;
		got = host_skb_pull_and_merge(&skb, src, v->pull_len);
		if (got != v->expect_int)
			goto fail;
		host_hex_decode(v->expect_hex, expect, sizeof(expect), &n);
		if ((size_t)skb.len != n || memcmp(buf, expect, n))
			goto fail;
	} else {
		return 1;
	}

	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t count = 0;
	int bad = 0;
	const char *path = argc > 1 ? argv[1] : "nat25_tlv_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &count))
		return 1;
	for (size_t i = 0; i < count; i++)
		bad |= run_one(&vecs[i]);
	return bad ? 1 : 0;
}
