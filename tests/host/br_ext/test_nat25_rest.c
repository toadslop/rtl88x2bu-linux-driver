// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_br_ext_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[16];
	char ip_hex[40];
	char expect_hex[48];
	host_jiffies_t jiffies;
	unsigned long ageing;
	int expect_int;
};

_adapter g_adapter;
struct nat25_network_db_entry g_fdb;

void host_nat25_gen_ipv6(u8 *na, const u8 *ip16);
int host_nat25_network_hash(u8 *na);
unsigned long host_nat25_timeout(_adapter *priv);
int host_nat25_has_expired(_adapter *priv, struct nat25_network_db_entry *fdb);

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "ip_hex", v->ip_hex, sizeof(v->ip_hex));
	host_json_parse_string_in(obj, len, "expect_hex", v->expect_hex,
				  sizeof(v->expect_hex));
	host_json_parse_int_in(obj, len, "jiffies", (int *)&v->jiffies);
	host_json_parse_int_in(obj, len, "ageing", (int *)&v->ageing);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_one(struct vector *v)
{
	u8 na[MAX_NETWORK_ADDR_LEN], expect[MAX_NETWORK_ADDR_LEN], ip[16];
	size_t n = 0;
	int got;

	host_br_ext_jiffies_val = v->jiffies;

	if (!strcmp(v->op, "gen_ipv6")) {
		host_hex_decode(v->ip_hex, ip, 16, &n);
		host_nat25_gen_ipv6(na, ip);
		got = host_nat25_network_hash(na);
		host_hex_decode(v->expect_hex, expect, sizeof(expect), &n);
		if (memcmp(na, expect, n) || got != v->expect_int)
			goto fail;
	} else if (!strcmp(v->op, "has_expired")) {
		g_fdb.ageing_timer = v->ageing;
		if (host_nat25_has_expired(&g_adapter, &g_fdb) != v->expect_int)
			goto fail;
	} else if (!strcmp(v->op, "timeout")) {
		if (host_nat25_timeout(&g_adapter) !=
		    v->jiffies - NAT25_AGEING_TIME * HZ)
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
	const char *path = argc > 1 ? argv[1] : "nat25_rest_vectors.json";

	if (host_load_vectors(path, vecs, sizeof(vecs[0]), 8, parse_vec, &count))
		return 1;
	for (size_t i = 0; i < count; i++)
		bad |= run_one(&vecs[i]);
	return bad ? 1 : 0;
}
