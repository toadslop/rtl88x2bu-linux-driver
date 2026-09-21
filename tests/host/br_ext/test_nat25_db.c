// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_br_ext_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[16];
	char mac_hex[16];
	char net_hex[48];
	char skb_mac_hex[16];
	char expect_mac_hex[16];
	host_jiffies_t jiffies;
	int expect_int;
};

_adapter g_adapter;

void host_nat25_db_network_insert(_adapter *priv, u8 *mac, u8 *net);
int host_nat25_db_network_lookup_and_replace(_adapter *priv,
					     struct host_sk_buff *skb, u8 *net);
int host_nat25_network_hash(u8 *na);

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "mac_hex", v->mac_hex, sizeof(v->mac_hex));
	host_json_parse_string_in(obj, len, "net_hex", v->net_hex, sizeof(v->net_hex));
	host_json_parse_string_in(obj, len, "skb_mac_hex", v->skb_mac_hex,
				  sizeof(v->skb_mac_hex));
	host_json_parse_string_in(obj, len, "expect_mac_hex", v->expect_mac_hex,
				  sizeof(v->expect_mac_hex));
	host_json_parse_int_in(obj, len, "jiffies", (int *)&v->jiffies);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int dec_net(const char *hex, u8 *out)
{
	size_t n = 0;

	return host_hex_decode(hex, out, MAX_NETWORK_ADDR_LEN, &n) ||
	       n != MAX_NETWORK_ADDR_LEN;
}

static int run_one(struct vector *v)
{
	u8 mac[ETH_ALEN], net[MAX_NETWORK_ADDR_LEN], skb_buf[ETH_HLEN];
	struct host_sk_buff skb = { .data = skb_buf, .len = ETH_HLEN };
	int got;

	host_br_ext_jiffies_val = v->jiffies;
	if (!strcmp(v->op, "reset"))
		memset(&g_adapter, 0, sizeof(g_adapter));
	else if (!strcmp(v->op, "insert")) {
		size_t n = 0;

		if (host_hex_decode(v->mac_hex, mac, ETH_ALEN, &n) || dec_net(v->net_hex, net))
			goto fail;
		host_nat25_db_network_insert(&g_adapter, mac, net);
	} else if (!strcmp(v->op, "lookup")) {
		if (dec_net(v->net_hex, net))
			goto fail;
		if (v->skb_mac_hex[0]) {
			size_t n = 0;

			if (host_hex_decode(v->skb_mac_hex, skb_buf, ETH_ALEN, &n))
				goto fail;
		}
		got = host_nat25_db_network_lookup_and_replace(&g_adapter, &skb, net);
		if (got != v->expect_int)
			goto fail;
		if (v->expect_mac_hex[0]) {
			u8 expect[ETH_ALEN];
			size_t n = 0;

			if (host_hex_decode(v->expect_mac_hex, expect, ETH_ALEN, &n) ||
			    memcmp(skb_buf, expect, ETH_ALEN))
				goto fail;
		}
	} else
		return 1;

	printf("PASS %s\n", v->name);
	return 0;
fail:
	fprintf(stderr, "FAIL %s\n", v->name);
	return 1;
}

int main(int argc, char **argv)
{
	struct vector vecs[12];
	size_t count = 0;
	int bad = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 12, parse_vec, &count))
		return 2;
	for (size_t i = 0; i < count; i++)
		bad += run_one(&vecs[i]);
	return bad ? 1 : 0;
}
