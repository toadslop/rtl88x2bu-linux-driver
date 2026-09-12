// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_bmc_rate_types.h"
#include "host_vector_json.h"

#ifdef RUST_AP_BMC_RATE_ORACLE
u8 rtw_ap_find_mini_tx_rate(struct _adapter *adapter)
{
	(void)adapter;
	return 0;
}
#endif

#define MAX_VECTORS 16
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char fn[32];
	u8 band, tx_rate, expect_rate;
	u8 n_stas;
	u8 sta_rates[HOST_BMC_MAX_STA];
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
	if (!host_json_parse_int_in(obj, len, "band", &tmp))
		v->band = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "tx_rate", &tmp))
		v->tx_rate = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_rate", &tmp))
		v->expect_rate = (u8)tmp;
	parse_sta_rates(obj, len, v);
	return 0;
}

static void setup_asoc_stas(struct _adapter *adapter, const struct vector *v)
{
	struct sta_info stas[HOST_BMC_MAX_STA];
	size_t i;

	_rtw_init_listhead(&adapter->stapriv.asoc_list);
	for (i = 0; i < v->n_stas; i++) {
		memset(&stas[i], 0, sizeof(stas[i]));
		stas[i].cmn.ra_info.curr_tx_rate = v->sta_rates[i];
		rtw_list_insert_tail(&stas[i].asoc_list, &adapter->stapriv.asoc_list);
	}
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	u8 got;

	memset(&adapter, 0, sizeof(adapter));
	adapter.hal_data.current_band_type = v->band;
	if (!strcmp(v->fn, "rtw_ap_find_bmc_rate")) {
		got = rtw_ap_find_bmc_rate(&adapter, v->tx_rate);
		if (got != v->expect_rate) {
			fprintf(stderr, "FAIL %s: expect %u got %u\n", v->name, v->expect_rate, got);
			return -1;
		}
		return 0;
	}
	if (!strcmp(v->fn, "rtw_ap_find_mini_tx_rate")) {
		setup_asoc_stas(&adapter, v);
		got = rtw_ap_find_mini_tx_rate(&adapter);
		if (got != v->expect_rate) {
			fprintf(stderr, "FAIL %s: expect %u got %u\n", v->name, v->expect_rate, got);
			return -1;
		}
		return 0;
	}
	fprintf(stderr, "FAIL %s: unknown fn %s\n", v->name, v->fn);
	return -1;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0, i, fail = 0, executed = 0, skipped = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++) {
#ifdef RUST_AP_BMC_RATE_ORACLE
		if (!strcmp(vectors[i].fn, "rtw_ap_find_mini_tx_rate")) {
			skipped++;
			continue;
		}
#endif
		executed++;
		if (run_vector(&vectors[i]))
			fail++;
	}
	if (fail) {
		printf("%zu vectors, %zu failures\n", executed, fail);
		return 1;
	}
#ifdef RUST_AP_BMC_RATE_ORACLE
	if (skipped)
		printf("%zu vectors, 0 failures (%zu mini skipped; oracle: rust/rtw_ap_rest.rs)\n",
		       executed, skipped);
	else
		printf("%zu vectors, 0 failures\n", executed);
#else
	(void)skipped;
	printf("%zu vectors, 0 failures\n", executed);
#endif
	return 0;
}
