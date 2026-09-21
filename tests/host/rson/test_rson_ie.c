// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rson_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char bssid_mac[20];
	char tlv_ie_hex[256];
	int expect_int;
};

static int fill_bss(WLAN_BSSID_EX *b, const char *mac_hex, const char *tlv_hex)
{
	size_t n = 0;
	u8 fixed[12] = {0};

	if (mac_hex[0] && host_hex_decode(mac_hex, b->MacAddress, ETH_ALEN, &n))
		return 1;
	memcpy(b->IEs, fixed, sizeof(fixed));
	b->IELength = sizeof(fixed);
	if (tlv_hex[0]) {
		if (host_hex_decode(tlv_hex, b->IEs + b->IELength,
				    MAX_IE_SZ - b->IELength, &n))
			return 1;
		b->IELength += (u32)n;
	}
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "bssid_mac", v->bssid_mac, sizeof(v->bssid_mac));
	host_json_parse_string_in(obj, len, "tlv_ie_hex", v->tlv_ie_hex, sizeof(v->tlv_ie_hex));
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	WLAN_BSSID_EX bss;
	struct rtw_rson_struct rs;
	u8 root[ETH_ALEN];
	size_t rn = 0;

	memset(&bss, 0, sizeof(bss));
	host_rson_set_root_bssid_count(0);
	if (!strcmp(v->name, "get_struct_root_bssid")) {
		if (host_hex_decode(v->bssid_mac, root, ETH_ALEN, &rn))
			return 1;
		host_rson_set_root_bssid(0, root);
		host_rson_set_root_bssid_count(1);
	}
	if (fill_bss(&bss, v->bssid_mac, v->tlv_ie_hex))
		return 1;
	if (rtw_get_rson_struct(&bss, &rs) != v->expect_int)
		return 1;
	if (v->expect_int == _TRUE && rs.id != CONFIG_RTW_REPEATER_SON_ID)
		return 1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[8];
	size_t count = 0, i;
	int fail = 0;

	if (argc < 2)
		return 2;
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), 8, parse_vec, &count))
		return 2;
	for (i = 0; i < count; i++)
		fail += run_vec(&vecs[i]);
	printf("%zu/%zu rson ie vectors PASS\n", count - (size_t)fail, count);
	return fail ? 1 : 0;
}
