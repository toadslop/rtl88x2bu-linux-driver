// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_wapi_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 20

struct vector {
	char name[64];
	char op[24];
	char pn1_hex[64];
	char pn2_hex[64];
	char mac_hex[24];
	int wapi_psk;
	int kid;
	int is_msk;
	int expect_u32;
	int expect_u8;
	int expect_ie_len;
	int expect_akm_suite;
};

static int parse_pn(const char *hex, u8 *pn)
{
	size_t n = 0;

	if (!hex[0])
		return 0;
	return host_hex_decode(hex, pn, 16, &n) || n != 16;
}

static int parse_mac(const char *hex, u8 *mac)
{
	size_t n = 0;

	if (!hex[0])
		return 0;
	return host_hex_decode(hex, mac, ETH_ALEN, &n) || n != ETH_ALEN;
}

static int count_used(_adapter *a)
{
	int c = 0;
	u8 i;

	for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++)
		if (a->wapiInfo.wapiCamEntry[i].IsUsed)
			c++;
	return c;
}

static int parse_vec(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op));
	host_json_parse_string_in(obj, len, "pn1_hex", v->pn1_hex, sizeof(v->pn1_hex));
	host_json_parse_string_in(obj, len, "pn2_hex", v->pn2_hex, sizeof(v->pn2_hex));
	host_json_parse_string_in(obj, len, "mac_hex", v->mac_hex, sizeof(v->mac_hex));
	host_json_parse_int_in(obj, len, "wapi_psk", &v->wapi_psk);
	host_json_parse_int_in(obj, len, "kid", &v->kid);
	host_json_parse_int_in(obj, len, "is_msk", &v->is_msk);
	host_json_parse_int_in(obj, len, "expect_u32", &v->expect_u32);
	host_json_parse_int_in(obj, len, "expect_u8", &v->expect_u8);
	host_json_parse_int_in(obj, len, "expect_ie_len", &v->expect_ie_len);
	host_json_parse_int_in(obj, len, "expect_akm_suite", &v->expect_akm_suite);
	return 0;
}

static int run_vec(struct vector *v)
{
	_adapter adapter;
	u8 pn1[16], pn2[16], mac[ETH_ALEN];
	u8 ret;

	host_wapi_adapter_init(&adapter);

	if (!strcmp(v->op, "compare_pn")) {
		u8 *p1 = NULL;
		u8 *p2 = NULL;

		if (v->pn1_hex[0]) {
			if (parse_pn(v->pn1_hex, pn1))
				return 1;
			p1 = pn1;
		}
		if (v->pn2_hex[0]) {
			if (parse_pn(v->pn2_hex, pn2))
				return 1;
			p2 = pn2;
		}
		if (WapiComparePN(p1, p2) != (u32)v->expect_u32)
			return 1;
	} else if (!strcmp(v->op, "set_ie")) {
		adapter.wapiInfo.bWapiPSK = (u8)v->wapi_psk;
		WapiSetIE(&adapter);
		if ((int)adapter.wapiInfo.wapiIELength != v->expect_ie_len)
			return 1;
		if (v->expect_akm_suite > 0 &&
		    adapter.wapiInfo.wapiIE[7] != (u8)v->expect_akm_suite)
			return 1;
	} else if (!strcmp(v->op, "cam_write")) {
		if (parse_mac(v->mac_hex, mac))
			return 1;
		ret = WapiGetEntryForCamWrite(&adapter, mac, (u8)v->kid, (u8)v->is_msk);
		if ((int)ret != v->expect_u8 || count_used(&adapter) != 1)
			return 1;
	} else if (!strcmp(v->op, "cam_write_twice")) {
		if (parse_mac(v->mac_hex, mac))
			return 1;
		ret = WapiGetEntryForCamWrite(&adapter, mac, (u8)v->kid, (u8)v->is_msk);
		if (ret != WapiGetEntryForCamWrite(&adapter, mac, (u8)v->kid, (u8)v->is_msk))
			return 1;
		if ((int)ret != v->expect_u8 || count_used(&adapter) != 1)
			return 1;
	} else if (!strcmp(v->op, "cam_clear_after_write")) {
		if (parse_mac(v->mac_hex, mac))
			return 1;
		(void)WapiGetEntryForCamWrite(&adapter, mac, (u8)v->kid, (u8)v->is_msk);
		ret = WapiGetEntryForCamClear(&adapter, mac, (u8)v->kid, (u8)v->is_msk);
		if ((int)ret != v->expect_u8 || count_used(&adapter) != 0)
			return 1;
	} else if (!strcmp(v->op, "cam_clear_miss")) {
		if (parse_mac(v->mac_hex, mac))
			return 1;
		if (WapiGetEntryForCamClear(&adapter, mac, (u8)v->kid, (u8)v->is_msk) != 0xff)
			return 1;
	} else if (!strcmp(v->op, "reset_all")) {
		u8 i;

		for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++)
			adapter.wapiInfo.wapiCamEntry[i].IsUsed = 1;
		WapiResetAllCamEntry(&adapter);
		if (count_used(&adapter) != 0)
			return 1;
	} else {
		return 1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t count = 0, i;
	int fail = 0;

	if (argc < 2)
		return 2;
	if (host_load_vectors(argv[1], vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vec, &count))
		return 2;
	for (i = 0; i < count; i++)
		fail += run_vec(&vecs[i]);
	printf("%zu/%zu wapi pn/cam vectors PASS\n", count - (size_t)fail, count);
	return fail ? 1 : 0;
}
