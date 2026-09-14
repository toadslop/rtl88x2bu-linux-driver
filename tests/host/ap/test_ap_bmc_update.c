// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>
#include "host_ap_bmc_rate_types.h"

#define MGN_UNKNOWN 0x00
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 48

struct vector {
	char name[MAX_NAME];
	char fn[36];
	int ap_mode, bmc_tx_rate, asoc_sta_count, seed_init_rate, expect_init_rate;
	u64 ramask;
	u8 n_stas, sta_rates[HOST_BMC_MAX_STA];
};

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
	if (!host_json_parse_int_in(obj, len, "expect_init_rate", &tmp))
		v->expect_init_rate = tmp;
	return parse_sta_rates(obj, len, v);
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
	} else {
		fprintf(stderr, "FAIL %s: unknown fn\n", v->name);
		return -1;
	}
	if (bcmc.init_rate != (u8)v->expect_init_rate) {
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
