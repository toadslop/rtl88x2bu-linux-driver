// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rson_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[24];
	int hopcnt;
	int connectible;
	int rssi;
	char mac_hex[20];
	char bssid_list_hex[128];
	int bssid_count;
	int expect_int;
};

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->rssi = -100;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_int_in(obj, len, "hopcnt", &v->hopcnt);
	host_json_parse_int_in(obj, len, "connectible", &v->connectible);
	host_json_parse_int_in(obj, len, "rssi", &v->rssi);
	host_json_parse_string_in(obj, len, "mac_hex", v->mac_hex, sizeof(v->mac_hex));
	host_json_parse_string_in(obj, len, "bssid_list_hex", v->bssid_list_hex,
				  sizeof(v->bssid_list_hex));
	host_json_parse_int_in(obj, len, "bssid_count", &v->bssid_count);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	if (!strcmp(v->op, "cal_score")) {
		struct rtw_rson_struct rs;

		memset(&rs, 0, sizeof(rs));
		rs.hopcnt = (u8)v->hopcnt;
		rs.connectible = (u8)v->connectible;
		return rtw_cal_rson_score(&rs, v->rssi) != (u8)v->expect_int;
	}
	if (!strcmp(v->op, "match_bssid")) {
		u8 mac[ETH_ALEN];
		u8 list[10][ETH_ALEN];
		size_t n = 0;
		int i;

		for (i = 0; i < v->bssid_count && i < 10; i++) {
			char pair[13];

			memcpy(pair, v->bssid_list_hex + i * 12, 12);
			pair[12] = '\0';
			if (host_hex_decode(pair, list[i], ETH_ALEN, &n))
				return 1;
		}
		if (host_hex_decode(v->mac_hex, mac, ETH_ALEN, &n))
			return 1;
		return is_match_bssid(mac, list, v->bssid_count) != v->expect_int;
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
	printf("%zu/%zu rson score vectors PASS\n", count - (size_t)fail, count);
	return fail ? 1 : 0;
}
