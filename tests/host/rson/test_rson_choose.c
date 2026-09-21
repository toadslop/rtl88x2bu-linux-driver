// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_rson_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	char op[24];
	char cand_mac[20];
	char cand_tlv[256];
	char comp_mac[20];
	char comp_tlv[256];
	int cand_rssi;
	int comp_rssi;
	int expect_int;
};

static struct wlan_network g_cand, g_comp;
static struct wlan_network *g_cand_ptr;
static _adapter g_adapter;

static int fill_bss(WLAN_BSSID_EX *b, const char *mac_hex, const char *tlv_hex, int rssi)
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
	b->Rssi = rssi;
	return 0;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	v->cand_rssi = -50;
	v->comp_rssi = -50;
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "cand_mac", v->cand_mac, sizeof(v->cand_mac));
	host_json_parse_string_in(obj, len, "cand_tlv", v->cand_tlv, sizeof(v->cand_tlv));
	host_json_parse_string_in(obj, len, "comp_mac", v->comp_mac, sizeof(v->comp_mac));
	host_json_parse_string_in(obj, len, "comp_tlv", v->comp_tlv, sizeof(v->comp_tlv));
	host_json_parse_int_in(obj, len, "cand_rssi", &v->cand_rssi);
	host_json_parse_int_in(obj, len, "comp_rssi", &v->comp_rssi);
	host_json_parse_int_in(obj, len, "expect_int", &v->expect_int);
	return 0;
}

static int run_vec(struct vector *v)
{
	if (!strcmp(v->op, "choose")) {
		memset(&g_cand, 0, sizeof(g_cand));
		memset(&g_comp, 0, sizeof(g_comp));
		host_rson_set_block_bssid_count(0);
		if (fill_bss(&g_cand.network, v->cand_mac, v->cand_tlv, v->cand_rssi))
			return 1;
		if (fill_bss(&g_comp.network, v->comp_mac, v->comp_tlv, v->comp_rssi))
			return 1;
		g_cand_ptr = &g_cand;
		return rtw_rson_choose(&g_cand_ptr, &g_comp) != v->expect_int;
	}
	if (!strcmp(v->op, "append_ie")) {
		u8 frame[64];
		u32 len = 0;

		memset(frame, 0, sizeof(frame));
		memset(&g_adapter, 0, sizeof(g_adapter));
		init_rtw_rson_data(&g_adapter.dvobj);
		g_adapter.dvobj.rson_data.hopcnt = 2;
		g_adapter.dvobj.rson_data.connectible = RTW_RSON_ALLOWCONNECT;
		rtw_rson_append_ie(&g_adapter, frame, &len);
		return (int)len != v->expect_int;
	}
	return 1;
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
	printf("%zu/%zu rson choose vectors PASS\n", count - (size_t)fail, count);
	return fail ? 1 : 0;
}
