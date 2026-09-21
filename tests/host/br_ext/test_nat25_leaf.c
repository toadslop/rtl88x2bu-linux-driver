// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_br_ext_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[16];
	char ip_hex[32];
	char mac_hex[24];
	char data_hex[48];
	char expect_hex[48];
	u32 sid;
	int expect_int;
};

void host_nat25_gen_ipv4(u8 *na, u32 ip);
void host_nat25_gen_pppoe(u8 *na, u8 *mac, u16 sid);
int host_nat25_network_hash(u8 *na);

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "ip_hex", v->ip_hex, sizeof(v->ip_hex));
	host_json_parse_string_in(obj, len, "mac_hex", v->mac_hex, sizeof(v->mac_hex));
	host_json_parse_string_in(obj, len, "data_hex", v->data_hex, sizeof(v->data_hex));
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex,
				  sizeof(v->expect_hex));
	host_json_parse_int_in(obj, len, "sid", (int *)&v->sid);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_one(struct vector *v)
{
	u8 na[MAX_NETWORK_ADDR_LEN], expect[MAX_NETWORK_ADDR_LEN];
	size_t n = 0;
	int got;

	if (!strcmp(v->op, "gen_ipv4")) {
		u32 ip = 0;

		host_hex_decode(v->ip_hex, (u8 *)&ip, 4, &n);
		host_nat25_gen_ipv4(na, ip);
	} else if (!strcmp(v->op, "gen_pppoe")) {
		u8 mac[6];

		host_hex_decode(v->mac_hex, mac, 6, &n);
		host_nat25_gen_pppoe(na, mac, (u16)v->sid);
	} else if (!strcmp(v->op, "hash_only")) {
		host_hex_decode(v->data_hex, na, sizeof(na), &n);
		got = host_nat25_network_hash(na);
		if (got != v->expect_int)
			goto fail;
		printf("PASS %s\n", v->name);
		return 0;
	} else {
		return 1;
	}

	got = host_nat25_network_hash(na);
	host_hex_decode(v->expect_hex, expect, sizeof(expect), &n);
	if (memcmp(na, expect, n) || got != v->expect_int)
		goto fail;
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
	const char *path = argc > 1 ? argv[1] : "nat25_leaf_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &count))
		return 1;
	for (size_t i = 0; i < count; i++)
		bad |= run_one(&vecs[i]);
	return bad ? 1 : 0;
}
