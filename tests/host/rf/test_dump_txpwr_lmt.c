// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for W3-58 dump_txpwr_lmt formatter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_dump_txpwr_lmt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_REGD_NAME 32
#define MAX_EXPECT 512

struct vector {
	char name[MAX_NAME];
	char regd_name[MAX_REGD_NAME];
	char expect_contains[MAX_EXPECT];
	char add_regd_name[MAX_REGD_NAME];
	u8 band_cap;
	u8 jaguar;
	u8 max_tx_cnt;
	u8 rfpath_num_2g;
	u8 cck_ofdm_state;
	u8 target_txpwr;
	u8 has_target_txpwr;
	u8 has_add;
	u8 add_band;
	u8 add_bw;
	u8 add_tlrs;
	u8 add_ntx_idx;
	u8 add_ch_idx;
	s8 add_lmt;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	host_json_parse_string_in(obj, len, "regd_name", v->regd_name,
				  sizeof(v->regd_name));
	host_json_parse_string_in(obj, len, "expect_contains", v->expect_contains,
				  sizeof(v->expect_contains));
	if (!host_json_parse_int_in(obj, len, "band_cap", &tmp))
		v->band_cap = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "jaguar", &tmp))
		v->jaguar = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "max_tx_cnt", &tmp))
		v->max_tx_cnt = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "rfpath_num_2g", &tmp))
		v->rfpath_num_2g = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "cck_ofdm_state", &tmp))
		v->cck_ofdm_state = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "target_txpwr", &tmp)) {
		v->target_txpwr = (u8)tmp;
		v->has_target_txpwr = 1;
	}
	if (!host_json_parse_string_in(obj, len, "add_regd_name", v->add_regd_name,
				       sizeof(v->add_regd_name)))
		v->has_add = 1;
	if (!host_json_parse_int_in(obj, len, "add_band", &tmp))
		v->add_band = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "add_bw", &tmp))
		v->add_bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "add_tlrs", &tmp))
		v->add_tlrs = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "add_ntx_idx", &tmp))
		v->add_ntx_idx = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "add_ch_idx", &tmp))
		v->add_ch_idx = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "add_lmt", &tmp))
		v->add_lmt = (s8)tmp;
	return 0;
}

static void apply_vector_config(struct _adapter *adapter, struct vector *v)
{
	if (v->regd_name[0])
		adapter->rf_ctl.regd_name = v->regd_name;
	if (v->band_cap)
		adapter->band_cap = v->band_cap;
	if (v->jaguar)
		adapter->jaguar = v->jaguar;
	if (v->max_tx_cnt)
		adapter->hal_data.max_tx_cnt = v->max_tx_cnt;
	if (v->rfpath_num_2g)
		host_rf_hal_spec_ptr()->rfpath_num_2g = v->rfpath_num_2g;
	if (v->cck_ofdm_state)
		adapter->rf_ctl.txpwr_lmt_2g_cck_ofdm_state = v->cck_ofdm_state;
	if (v->has_target_txpwr)
		host_rf_dump_txpwr_lmt_set_target_txpwr(v->target_txpwr);
}

static int run_vector(struct vector *v)
{
	struct _adapter adapter;

	host_rf_dump_txpwr_lmt_reset(&adapter);
	apply_vector_config(&adapter, v);

	if (v->has_add && v->add_regd_name[0]) {
		rtw_txpwr_lmt_add(&adapter.rf_ctl, v->add_regd_name, v->add_band,
				  v->add_bw, v->add_tlrs, v->add_ntx_idx,
				  v->add_ch_idx, v->add_lmt);
	}

	host_sel_reset();
	dump_txpwr_lmt(&host_sel_out, &adapter);

	if (v->expect_contains[0] &&
	    !strstr(host_sel_out.buf, v->expect_contains)) {
		fprintf(stderr, "%s: expected substring missing: %s\noutput:\n%s\n",
			v->name, v->expect_contains, host_sel_out.buf);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	int failed = 0;
	const char *path = "dump_txpwr_lmt_vectors.json";

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "load %s failed\n", path);
		return 1;
	}

	for (size_t i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i]))
			failed++;
	}

	if (failed) {
		fprintf(stderr, "FAIL %zu vectors (%d failures)\n", nvec, failed);
		return 1;
	}

	printf("PASS %zu vectors (oracle: core/rtw_rf_dump_txpwr_lmt.c dump_txpwr_lmt) (%s)\n",
	       nvec, path);
	return 0;
}

#ifdef RUST_RF_DUMP_TXPWR_LMT_ORACLE
/* Rust oracle links dump_txpwr_lmt from librust_rf_dump_txpwr_lmt.a */
#endif
