// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_mlme_rest BSSID/compare helpers (W3-53). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_mlme_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum mlme_fn {
	FN_CAP_FROM_IE, FN_GET_CAPABILITY, FN_TIMESTAMP_PTR, FN_BEACON_INTERVAL_PTR,
	FN_SAME_ESS, FN_SAME_NETWORK, FN_SAME_IBSS, FN_RANDOM_IBSS,
};

struct vector {
	char name[MAX_NAME];
	enum mlme_fn fn;
	char ie_hex[129];
	char ssid_a[33], ssid_b[33];
	char mac[13], mac_a[13], mac_b[13];
	u16 cap_a, cap_b;
	u8 feature;
	int ssid_a_len, ssid_b_len;
	int adapter_privacy, network_privacy, expect_offset, expect_u16, expect;
	u32 random32;
	char expect_bssid_hex[32];
};

extern void host_mlme_set_random32(u32 val);

static int fail(struct vector *v, const char *why)
{
	fprintf(stderr, "%s: %s\n", v->name, why);
	return -1;
}

static int parse_fn(const char *obj, size_t len, enum mlme_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "cap_from_ie")) *out = FN_CAP_FROM_IE;
	else if (!strcmp(fn, "get_capability")) *out = FN_GET_CAPABILITY;
	else if (!strcmp(fn, "timestamp_ptr")) *out = FN_TIMESTAMP_PTR;
	else if (!strcmp(fn, "beacon_interval_ptr")) *out = FN_BEACON_INTERVAL_PTR;
	else if (!strcmp(fn, "same_ess")) *out = FN_SAME_ESS;
	else if (!strcmp(fn, "same_network")) *out = FN_SAME_NETWORK;
	else if (!strcmp(fn, "same_ibss")) *out = FN_SAME_IBSS;
	else if (!strcmp(fn, "random_ibss")) *out = FN_RANDOM_IBSS;
	else return -1;
	return 0;
}

static void fill_ssid(WLAN_BSSID_EX *bss, const char *ssid, int ssid_len_override)
{
	if (ssid_len_override >= 0) {
		bss->Ssid.SsidLength = (u32)ssid_len_override;
		if (ssid && ssid[0])
			memcpy(bss->Ssid.Ssid, ssid, bss->Ssid.SsidLength);
		return;
	}
	bss->Ssid.SsidLength = (u32)strlen(ssid);
	memcpy(bss->Ssid.Ssid, ssid, bss->Ssid.SsidLength);
}

static void fill_bss(WLAN_BSSID_EX *bss, u16 cap, const char *mac_hex, const char *ssid,
		     int ssid_len_override)
{
	u8 ie[12] = {0}, mac[ETH_ALEN];
	size_t n = 0;

	memset(bss, 0, sizeof(*bss));
	bss->IELength = 12;
	if (mac_hex && mac_hex[0] && !host_hex_decode(mac_hex, mac, sizeof(mac), &n))
		memcpy(bss->MacAddress, mac, ETH_ALEN);
	ie[8] = 1; ie[10] = cap & 0xff; ie[11] = cap >> 8;
	memcpy(bss->IEs, ie, sizeof(ie));
	fill_ssid(bss, ssid, ssid_len_override);
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_string_in(obj, len, "ie_hex", v->ie_hex, sizeof(v->ie_hex));
	host_json_parse_string_in(obj, len, "ssid_a", v->ssid_a, sizeof(v->ssid_a));
	host_json_parse_string_in(obj, len, "ssid_b", v->ssid_b, sizeof(v->ssid_b));
	host_json_parse_string_in(obj, len, "mac", v->mac, sizeof(v->mac));
	host_json_parse_string_in(obj, len, "mac_a", v->mac_a, sizeof(v->mac_a));
	host_json_parse_string_in(obj, len, "mac_b", v->mac_b, sizeof(v->mac_b));
	host_json_parse_string_in(obj, len, "expect_bssid_hex", v->expect_bssid_hex,
				  sizeof(v->expect_bssid_hex));
	if (!host_json_parse_int_in(obj, len, "cap_a", &tmp)) v->cap_a = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "cap_b", &tmp)) v->cap_b = (u16)tmp;
	if (!host_json_parse_int_in(obj, len, "feature", &tmp)) v->feature = (u8)tmp;
	v->ssid_a_len = -1;
	v->ssid_b_len = -1;
	host_json_parse_int_in(obj, len, "ssid_a_len", &v->ssid_a_len);
	host_json_parse_int_in(obj, len, "ssid_b_len", &v->ssid_b_len);
	host_json_parse_int_in(obj, len, "adapter_privacy", &v->adapter_privacy);
	host_json_parse_int_in(obj, len, "network_privacy", &v->network_privacy);
	if (host_json_parse_int_in(obj, len, "random32", (int *)&v->random32)) v->random32 = 0;
	host_json_parse_int_in(obj, len, "expect_offset", &v->expect_offset);
	host_json_parse_int_in(obj, len, "expect_u16", &v->expect_u16);
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 ie[12], expect[ETH_ALEN];
	size_t n = 0;
	WLAN_BSSID_EX a, b;
	struct wlan_network net;
	_adapter adapter;

	switch (v->fn) {
	case FN_CAP_FROM_IE:
	case FN_TIMESTAMP_PTR:
	case FN_BEACON_INTERVAL_PTR:
		if (host_hex_decode(v->ie_hex, ie, sizeof(ie), &n) || n != 12)
			return fail(v, "ie decode");
		if (v->fn == FN_CAP_FROM_IE &&
		    (int)(rtw_get_capability_from_ie(ie) - ie) != v->expect_offset)
			return fail(v, "cap offset");
		if (v->fn == FN_TIMESTAMP_PTR &&
		    (int)(rtw_get_timestampe_from_ie(ie) - ie) != v->expect_offset)
			return fail(v, "timestamp offset");
		if (v->fn == FN_BEACON_INTERVAL_PTR &&
		    (int)(rtw_get_beacon_interval_from_ie(ie) - ie) != v->expect_offset)
			return fail(v, "beacon interval offset");
		break;
	case FN_GET_CAPABILITY:
		fill_bss(&a, 1, NULL, "", -1);
		if (host_hex_decode(v->ie_hex, a.IEs, sizeof(a.IEs), &n) || n != 12)
			return fail(v, "ie decode");
		if (rtw_get_capability(&a) != (u16)v->expect_u16)
			return fail(v, "capability");
		break;
	case FN_SAME_ESS:
		fill_bss(&a, 0, NULL, v->ssid_a, -1);
		fill_bss(&b, 0, NULL, v->ssid_b, -1);
		if (is_same_ess(&a, &b) != v->expect)
			return fail(v, "same_ess");
		break;
	case FN_SAME_NETWORK:
		fill_bss(&a, v->cap_a, v->mac_a[0] ? v->mac_a : v->mac, v->ssid_a, v->ssid_a_len);
		fill_bss(&b, v->cap_b, v->mac_b[0] ? v->mac_b : v->mac, v->ssid_b, v->ssid_b_len);
		if (is_same_network(&a, &b, v->feature) != v->expect)
			return fail(v, "same_network");
		break;
	case FN_SAME_IBSS:
		memset(&adapter, 0, sizeof(adapter));
		memset(&net, 0, sizeof(net));
		adapter.securitypriv.dot11PrivacyAlgrthm = (u32)v->adapter_privacy;
		net.network.Privacy = (u32)v->network_privacy;
		if (rtw_is_same_ibss(&adapter, &net) != v->expect)
			return fail(v, "same_ibss");
		break;
	case FN_RANDOM_IBSS:
		host_mlme_set_random32(v->random32);
		rtw_generate_random_ibss(ie);
		if (host_hex_decode(v->expect_bssid_hex, expect, sizeof(expect), &n) ||
		    n != ETH_ALEN || memcmp(ie, expect, ETH_ALEN))
			return fail(v, "random ibss");
		break;
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec, i;

	if (argc != 2)
		return fprintf(stderr, "usage: %s mlme_vectors.json\n", argv[0]), 1;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec))
		return fprintf(stderr, "failed to load %s\n", argv[1]), 1;
	for (i = 0; i < nvec; i++)
		if (run_vector(&vectors[i]))
			return 1;
#ifndef RUST_MLME_ORACLE
	printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c) (%s)\n", nvec, argv[1]);
#else
	printf("PASS %zu vectors (oracle: rust/rtw_mlme_rest.rs) (%s)\n", nvec, argv[1]);
#endif
	return 0;
}
