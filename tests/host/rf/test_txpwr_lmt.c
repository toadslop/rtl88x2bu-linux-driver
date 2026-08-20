// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for txpwr_lmt list CRUD (W3-52).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_txpwr_lmt_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_REGD_NAME 32

enum txpwr_lmt_fn {
	FN_ADD_GET = 0,
	FN_GET,
	FN_ADD_MERGE,
	FN_MULTI_ADD,
	FN_ADD_FREE,
	FN_INIT_CHECK,
};

struct vector {
	char name[MAX_NAME];
	enum txpwr_lmt_fn fn;
	char regd_name[MAX_REGD_NAME];
	char regd_name1[MAX_REGD_NAME];
	char regd_name2[MAX_REGD_NAME];
	u8 band;
	u8 bw;
	u8 tlrs;
	u8 ntx_idx;
	u8 ch_idx;
	s8 lmt;
	s8 lmt_first;
	s8 lmt_second;
	int expect_num;
	int expect_found;
	int expect_num_after_add;
	int expect_num_after_free;
	s8 expect_lmt;
};

static int parse_fn(const char *obj, size_t len, enum txpwr_lmt_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "add_get"))
		*out = FN_ADD_GET;
	else if (!strcmp(fn, "get"))
		*out = FN_GET;
	else if (!strcmp(fn, "add_merge"))
		*out = FN_ADD_MERGE;
	else if (!strcmp(fn, "multi_add"))
		*out = FN_MULTI_ADD;
	else if (!strcmp(fn, "add_free"))
		*out = FN_ADD_FREE;
	else if (!strcmp(fn, "init_check"))
		*out = FN_INIT_CHECK;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	host_json_parse_string_in(obj, len, "regd_name", v->regd_name,
				  sizeof(v->regd_name));
	host_json_parse_string_in(obj, len, "regd_name1", v->regd_name1,
				  sizeof(v->regd_name1));
	host_json_parse_string_in(obj, len, "regd_name2", v->regd_name2,
				  sizeof(v->regd_name2));
	if (!host_json_parse_int_in(obj, len, "band", &tmp))
		v->band = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "bw", &tmp))
		v->bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "tlrs", &tmp))
		v->tlrs = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ntx_idx", &tmp))
		v->ntx_idx = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ch_idx", &tmp))
		v->ch_idx = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "lmt", &tmp))
		v->lmt = (s8)tmp;
	if (!host_json_parse_int_in(obj, len, "lmt_first", &tmp))
		v->lmt_first = (s8)tmp;
	if (!host_json_parse_int_in(obj, len, "lmt_second", &tmp))
		v->lmt_second = (s8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_lmt", &tmp))
		v->expect_lmt = (s8)tmp;
	host_json_parse_int_in(obj, len, "expect_found", &v->expect_found);
	host_json_parse_int_in(obj, len, "expect_num", &v->expect_num);
	host_json_parse_int_in(obj, len, "expect_num_after_add",
			       &v->expect_num_after_add);
	host_json_parse_int_in(obj, len, "expect_num_after_free",
			       &v->expect_num_after_free);
	return 0;
}

static s8 read_lmt(struct txpwr_lmt_ent *ent, u8 band, u8 bw, u8 tlrs,
		   u8 ch_idx, u8 ntx_idx)
{
	if (band == BAND_ON_2_4G)
		return ent->lmt_2g[bw][tlrs][ch_idx][ntx_idx];
#if CONFIG_IEEE80211_BAND_5GHZ
	if (band == BAND_ON_5G)
		return ent->lmt_5g[bw][tlrs - 1][ch_idx][ntx_idx];
#endif
	return 0;
}

static int check_get(struct vector *v, struct rf_ctl_t *rfctl)
{
	struct txpwr_lmt_ent *ent;

	ent = rtw_txpwr_lmt_get_by_name(rfctl, v->regd_name);
	if (v->expect_found) {
		if (!ent) {
			fprintf(stderr, "%s: expected match, got NULL\n", v->name);
			return -1;
		}
		if (strcmp(ent->regd_name, v->regd_name) != 0) {
			fprintf(stderr, "%s: got name '%s' expect '%s'\n",
				v->name, ent->regd_name, v->regd_name);
			return -1;
		}
	} else if (ent) {
		fprintf(stderr, "%s: expected miss, got '%s'\n", v->name,
			ent->regd_name);
		return -1;
	}
	return 0;
}

static int run_vector(struct vector *v)
{
	struct rf_ctl_t rfctl;
	struct txpwr_lmt_ent *ent;

	host_rf_txpwr_lmt_reset(&rfctl);

	switch (v->fn) {
	case FN_ADD_GET:
		rtw_txpwr_lmt_add(&rfctl, v->regd_name, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt);
		if ((int)rfctl.txpwr_regd_num != v->expect_num) {
			fprintf(stderr, "%s: txpwr_regd_num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num, v->expect_num);
			return -1;
		}
		return check_get(v, &rfctl);
	case FN_GET:
		if ((int)rfctl.txpwr_regd_num != v->expect_num) {
			fprintf(stderr, "%s: txpwr_regd_num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num, v->expect_num);
			return -1;
		}
		return check_get(v, &rfctl);
	case FN_ADD_MERGE:
		rtw_txpwr_lmt_add(&rfctl, v->regd_name, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt_first);
		rtw_txpwr_lmt_add(&rfctl, v->regd_name, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt_second);
		if ((int)rfctl.txpwr_regd_num != v->expect_num) {
			fprintf(stderr, "%s: txpwr_regd_num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num, v->expect_num);
			return -1;
		}
		ent = rtw_txpwr_lmt_get_by_name(&rfctl, v->regd_name);
		if (!ent) {
			fprintf(stderr, "%s: entry missing after merge\n", v->name);
			return -1;
		}
		if (read_lmt(ent, v->band, v->bw, v->tlrs, v->ch_idx, v->ntx_idx) !=
		    v->expect_lmt) {
			fprintf(stderr, "%s: lmt=%d expect=%d\n", v->name,
				read_lmt(ent, v->band, v->bw, v->tlrs, v->ch_idx,
					 v->ntx_idx),
				v->expect_lmt);
			return -1;
		}
		break;
	case FN_MULTI_ADD:
		rtw_txpwr_lmt_add(&rfctl, v->regd_name1, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt);
		rtw_txpwr_lmt_add(&rfctl, v->regd_name2, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt);
		if ((int)rfctl.txpwr_regd_num != v->expect_num) {
			fprintf(stderr, "%s: txpwr_regd_num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num, v->expect_num);
			return -1;
		}
		break;
	case FN_ADD_FREE:
		rtw_txpwr_lmt_add(&rfctl, v->regd_name, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, v->lmt);
		if ((int)rfctl.txpwr_regd_num != v->expect_num_after_add) {
			fprintf(stderr, "%s: after add num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num,
				v->expect_num_after_add);
			return -1;
		}
		rtw_txpwr_lmt_list_free(&rfctl);
		if ((int)rfctl.txpwr_regd_num != v->expect_num_after_free) {
			fprintf(stderr, "%s: after free num=%u expect=%d\n",
				v->name, rfctl.txpwr_regd_num,
				v->expect_num_after_free);
			return -1;
		}
		break;
	case FN_INIT_CHECK:
		rtw_txpwr_lmt_add(&rfctl, v->regd_name, v->band, v->bw, v->tlrs,
				  v->ntx_idx, v->ch_idx, host_rf_hal_spec_ptr()->txgi_max);
		ent = rtw_txpwr_lmt_get_by_name(&rfctl, v->regd_name);
		if (!ent) {
			fprintf(stderr, "%s: entry missing\n", v->name);
			return -1;
		}
		if (read_lmt(ent, v->band, v->bw, v->tlrs, v->ch_idx, v->ntx_idx) !=
		    v->expect_lmt) {
			fprintf(stderr, "%s: init lmt=%d expect=%d\n", v->name,
				read_lmt(ent, v->band, v->bw, v->tlrs, v->ch_idx,
					 v->ntx_idx),
				v->expect_lmt);
			return -1;
		}
		break;
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vecs[MAX_VECTORS];
	size_t count = 0;
	size_t executed = 0;
	size_t i;
	const char *path = "txpwr_lmt_vectors.json";

	if (argc > 1)
		path = argv[1];
	if (host_load_vectors(path, vecs, sizeof(vecs[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (i = 0; i < count; i++) {
		executed++;
		if (run_vector(&vecs[i])) {
			fprintf(stderr, "FAIL %s\n", vecs[i].name);
			return 1;
		}
	}
#ifdef RUST_RF_TXPWR_LMT_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_rf_rest.rs txpwr_lmt) (%s)\n",
	       executed, path);
#else
	printf("PASS %zu vectors (%s)\n", executed, path);
#endif
	return 0;
}
