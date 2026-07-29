// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_ieee80211_rest rate helpers (W3-26).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_RATES 16
#define MAX_IES 256
#define MAX_RATES_EX 16

enum ie_rest_fn {
	FN_BIT_VALUE = 0,
	FN_CHECK_NETWORK_TYPE,
	FN_SET_SUPPORTED_RATE,
	FN_FILTER_SUPPORT_RATEIE,
	FN_UPDATE_RATE_BYMODE,
};

struct vector {
	char name[MAX_NAME];
	enum ie_rest_fn fn;
	u8 rates[MAX_RATES];
	int ratelen;
	int channel;
	int mode;
	int keep;
	int expect;
	u8 expect_rates[MAX_RATES_EX];
	size_t expect_rates_len;
	u8 ies[MAX_IES];
	size_t ies_len;
	u32 ds_config;
	u8 expect_ies[MAX_IES];
	size_t expect_ies_len;
	u32 expect_ielength;
	u8 expect_supported[MAX_RATES_EX];
	size_t expect_supported_len;
};

int rtw_get_bit_value_from_ieee_value(u8 val);
int rtw_check_network_type(unsigned char *rate, int ratelen, int channel);
void rtw_set_supported_rate(u8 *SupportedRates, unsigned int mode);
void rtw_filter_suppport_rateie(WLAN_BSSID_EX *pbss_network, u8 keep);
u8 rtw_update_rate_bymode(WLAN_BSSID_EX *pbss_network, u32 mode);

static int parse_fn(const char *obj, size_t obj_len, enum ie_rest_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_get_bit_value_from_ieee_value") == 0)
		*out = FN_BIT_VALUE;
	else if (strcmp(fn, "rtw_check_network_type") == 0)
		*out = FN_CHECK_NETWORK_TYPE;
	else if (strcmp(fn, "rtw_set_supported_rate") == 0)
		*out = FN_SET_SUPPORTED_RATE;
	else if (strcmp(fn, "rtw_filter_suppport_rateie") == 0)
		*out = FN_FILTER_SUPPORT_RATEIE;
	else if (strcmp(fn, "rtw_update_rate_bymode") == 0)
		*out = FN_UPDATE_RATE_BYMODE;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "ratelen", &v->ratelen);
	host_json_parse_int_in(obj, obj_len, "channel", &v->channel);
	host_json_parse_int_in(obj, obj_len, "mode", &v->mode);
	host_json_parse_int_in(obj, obj_len, "keep", &v->keep);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	host_json_parse_int_in(obj, obj_len, "ds_config", (int *)&v->ds_config);
	host_json_parse_int_in(obj, obj_len, "expect_ielength", (int *)&v->expect_ielength);
	{
		char hex[HOST_VECTOR_MAX_HEX_BUF];
		size_t rates_len = 0;

		if (!host_json_parse_string_in(obj, obj_len, "rates", hex, sizeof(hex))) {
			if (host_hex_decode(hex, v->rates, sizeof(v->rates), &rates_len))
				return -1;
			v->ratelen = (int)rates_len;
		}
		if (!host_json_parse_string_in(obj, obj_len, "expect_rates", hex, sizeof(hex))) {
			if (host_hex_decode(hex, v->expect_rates, sizeof(v->expect_rates),
					    &v->expect_rates_len))
				return -1;
		}
		if (!host_json_parse_string_in(obj, obj_len, "ies", hex, sizeof(hex))) {
			if (host_hex_decode(hex, v->ies, sizeof(v->ies), &v->ies_len))
				return -1;
		}
		if (!host_json_parse_string_in(obj, obj_len, "expect_ies", hex, sizeof(hex))) {
			if (host_hex_decode(hex, v->expect_ies, sizeof(v->expect_ies),
					    &v->expect_ies_len))
				return -1;
		}
		if (!host_json_parse_string_in(obj, obj_len, "expect_supported", hex,
					       sizeof(hex))) {
			if (host_hex_decode(hex, v->expect_supported,
					    sizeof(v->expect_supported),
					    &v->expect_supported_len))
				return -1;
		}
	}
	return 0;
}

static int rates_match(const u8 *got, size_t got_len, const u8 *expect,
		       size_t expect_len)
{
	size_t i;

	if (got_len != expect_len)
		return 0;
	for (i = 0; i < expect_len; i++) {
		if (got[i] != expect[i])
			return 0;
	}
	return 1;
}

static WLAN_BSSID_EX make_bss(const struct vector *v)
{
	WLAN_BSSID_EX bss;

	memset(&bss, 0, sizeof(bss));
	bss.Configuration.DSConfig = v->ds_config;
	bss.IELength = (u32)v->ies_len;
	memcpy(bss.IEs, v->ies, v->ies_len);
	return bss;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_BIT_VALUE: {
		int ret = rtw_get_bit_value_from_ieee_value(v->rates[0]);

		if (ret != v->expect) {
			fprintf(stderr, "%s: bit_value got %d expect %d\n", v->name,
				ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_CHECK_NETWORK_TYPE: {
		int ret = rtw_check_network_type(v->rates, v->ratelen, v->channel);

		if (ret != v->expect) {
			fprintf(stderr, "%s: network_type got %d expect %d\n", v->name,
				ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_SET_SUPPORTED_RATE: {
		u8 supported[NDIS_802_11_LENGTH_RATES_EX];

		memset(supported, 0xff, sizeof(supported));
		rtw_set_supported_rate(supported, (unsigned int)v->mode);
		if (!rates_match(supported, v->expect_rates_len, v->expect_rates,
				 v->expect_rates_len)) {
			fprintf(stderr, "%s: supported_rates mismatch\n", v->name);
			return -1;
		}
		break;
	}
	case FN_FILTER_SUPPORT_RATEIE: {
		WLAN_BSSID_EX bss = make_bss(v);

		rtw_filter_suppport_rateie(&bss, (u8)v->keep);
		if (bss.IELength != v->expect_ielength ||
		    !rates_match(bss.IEs, v->expect_ies_len, v->expect_ies,
				 v->expect_ies_len)) {
			fprintf(stderr, "%s: filter_rateie mismatch len=%u\n", v->name,
				bss.IELength);
			return -1;
		}
		break;
	}
	case FN_UPDATE_RATE_BYMODE: {
		WLAN_BSSID_EX bss = make_bss(v);
		u8 ret = rtw_update_rate_bymode(&bss, (u32)v->mode);

		if ((int)ret != v->expect ||
		    bss.IELength != v->expect_ielength ||
		    !rates_match(bss.IEs, v->expect_ies_len, v->expect_ies,
				 v->expect_ies_len) ||
		    !rates_match(bss.SupportedRates, v->expect_supported_len,
				 v->expect_supported, v->expect_supported_len)) {
			fprintf(stderr, "%s: update_rate_bymode mismatch ret=%u len=%u\n",
				v->name, ret, bss.IELength);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "ie_rest_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
	printf("all %zu ieee80211_rest vectors passed (oracle: core/rtw_ieee80211_rest.c)\n",
	       nvec);
	return 0;
}
