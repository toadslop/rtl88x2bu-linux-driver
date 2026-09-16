// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_bmc_update_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 48

struct vector {
	char name[MAX_NAME];
	char fn[36];
	int ap_mode, bmc_tx_rate, asoc_sta_count, seed_init_rate, expect_init_rate;
	int wireless_mode, ds_config, expect_wireless_mode, expect_state, expect_media_rpt;
	u8 has_expect_init_rate, has_expect_wireless_mode, has_expect_state,
		has_expect_media_rpt;
	u64 ramask;
	u8 n_stas, sta_rates[HOST_BMC_MAX_STA];
	u8 n_rates, supported_rates[HOST_BMC_MAX_RATES];
};

static int parse_supported_rates(const char *obj, size_t len, struct vector *v);

static int parse_sta_rates(const char *obj, size_t len, struct vector *v)
{
	int tmp, i;
	char key[16];

	v->n_stas = 0;
	for (i = 0; i < HOST_BMC_MAX_STA; i++) {
		snprintf(key, sizeof(key), "sta_rate_%d", i + 1);
		if (host_json_parse_int_in(obj, len, key, &tmp))
			break;
		v->sta_rates[v->n_stas++] = (u8)tmp;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "ap_mode", &tmp))
		v->ap_mode = tmp;
	if (!host_json_parse_int_in(obj, len, "bmc_tx_rate", &tmp))
		v->bmc_tx_rate = tmp;
	if (!host_json_parse_int_in(obj, len, "ramask", &tmp))
		v->ramask = (u64)tmp;
	if (!host_json_parse_int_in(obj, len, "asoc_sta_count", &tmp))
		v->asoc_sta_count = tmp;
	if (!host_json_parse_int_in(obj, len, "seed_init_rate", &tmp))
		v->seed_init_rate = tmp;
	if (!host_json_parse_int_in(obj, len, "expect_init_rate", &tmp)) {
		v->expect_init_rate = tmp;
		v->has_expect_init_rate = 1;
	}
	if (!host_json_parse_int_in(obj, len, "wireless_mode", &tmp))
		v->wireless_mode = tmp;
	if (!host_json_parse_int_in(obj, len, "ds_config", &tmp))
		v->ds_config = tmp;
	if (!host_json_parse_int_in(obj, len, "expect_wireless_mode", &tmp)) {
		v->expect_wireless_mode = tmp;
		v->has_expect_wireless_mode = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_state", &tmp)) {
		v->expect_state = tmp;
		v->has_expect_state = 1;
	}
	if (!host_json_parse_int_in(obj, len, "expect_media_rpt", &tmp)) {
		v->expect_media_rpt = tmp;
		v->has_expect_media_rpt = 1;
	}
	parse_sta_rates(obj, len, v);
	return parse_supported_rates(obj, len, v);
}

static int parse_supported_rates(const char *obj, size_t len, struct vector *v)
{
	const char *p = host_json_find_key_in(obj, len, "supported_rates");

	v->n_rates = 0;
	if (!p)
		return 0;
	p = host_json_skip_ws(p);
	if (*p != '[')
		return 0;
	p++;
	while (v->n_rates < HOST_BMC_MAX_RATES) {
		p = host_json_skip_ws(p);
		if (*p == ']')
			break;
		v->supported_rates[v->n_rates++] = (u8)strtol(p, (char **)&p, 10);
		p = host_json_skip_ws(p);
		if (*p == ',')
			p++;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	struct sta_info bcmc, asoc[HOST_BMC_MAX_STA];
	size_t i;

	memset(&adapter, 0, sizeof(adapter));
	memset(&bcmc, 0, sizeof(bcmc));
	if (v->ap_mode)
		adapter.mlmepriv.state = WIFI_AP_STATE;
	adapter.mlmeextpriv.cur_wireless_mode = (u32)v->wireless_mode;
	adapter.bmc_tx_rate = (u8)v->bmc_tx_rate;
	adapter.stapriv.asoc_sta_count = v->asoc_sta_count;
	adapter.stapriv.host_bcmc_sta = &bcmc;
	bcmc.init_rate = (u8)v->seed_init_rate;
	bcmc.cmn.ra_info.ramask = v->ramask;

	if (!strcmp(v->fn, "rtw_update_bmc_sta_tx_rate")) {
		_rtw_init_listhead(&adapter.stapriv.asoc_list);
		for (i = 0; i < v->n_stas; i++) {
			memset(&asoc[i], 0, sizeof(asoc[i]));
			asoc[i].cmn.ra_info.curr_tx_rate = v->sta_rates[i];
			rtw_list_insert_tail(&asoc[i].asoc_list, &adapter.stapriv.asoc_list);
		}
		rtw_update_bmc_sta_tx_rate(&adapter);
	} else if (!strcmp(v->fn, "rtw_init_bmc_sta_tx_rate")) {
		rtw_init_bmc_sta_tx_rate(&adapter, &bcmc);
	} else if (!strcmp(v->fn, "update_bmc_sta")) {
		size_t ri;

		host_ap_bmc_sta_reset_hooks();
		adapter.mlmepriv.cur_network.network.Configuration.DSConfig =
			(u8)v->ds_config;
		for (ri = 0; ri < v->n_rates; ri++)
			adapter.mlmepriv.cur_network.network.SupportedRates[ri] =
				v->supported_rates[ri];
		bcmc.cmn.ra_info.ramask = v->ramask;
		update_bmc_sta(&adapter);
		if (v->has_expect_wireless_mode &&
		    bcmc.wireless_mode != (u8)v->expect_wireless_mode) {
			fprintf(stderr, "FAIL %s: wireless_mode expect %d got %u\n",
				v->name, v->expect_wireless_mode, bcmc.wireless_mode);
			return -1;
		}
		if (v->has_expect_state && bcmc.state != (u8)v->expect_state) {
			fprintf(stderr, "FAIL %s: state expect %d got %u\n", v->name,
				v->expect_state, bcmc.state);
			return -1;
		}
		if (v->has_expect_media_rpt &&
		    host_ap_bmc_sta_media_rpt_count() != (u8)v->expect_media_rpt) {
			fprintf(stderr, "FAIL %s: media_rpt expect %d got %u\n",
				v->name, v->expect_media_rpt,
				host_ap_bmc_sta_media_rpt_count());
			return -1;
		}
	} else {
		fprintf(stderr, "FAIL %s: unknown fn\n", v->name);
		return -1;
	}
	if (v->has_expect_init_rate &&
	    bcmc.init_rate != (u8)v->expect_init_rate) {
		fprintf(stderr, "FAIL %s: expect %d got %u\n", v->name,
			v->expect_init_rate, bcmc.init_rate);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0, i, fail = 0;

	if (argc != 2 || host_load_vectors(argv[1], vectors, sizeof(vectors[0]),
					   MAX_VECTORS, parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&vectors[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
