// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_chplan.c lookup, DFS, and country helpers
 * (W2-17a/W2-18/W2-19).
 *
 * oracle: core/rtw_chplan.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_chplan_types.h"
#include "host_vector_json.h"
#include "rtw_chplan.h"

#ifndef REGD_SRC_RTK_PRIV
#define REGD_SRC_RTK_PRIV 0
#endif

u8 rtw_chplan_get_default_regd_2g(u8 id);
u8 rtw_chplan_get_default_regd_5g(u8 id);

#define MAX_VECTORS 64
#define MAX_NAME 128

enum chplan_fn {
	FN_REGD_2G = 0,
	FN_REGD_5G,
	FN_REGD,
	FN_IS_EMPTY,
	FN_IS_VALID,
	FN_EXCL_CHS,
	FN_DFS_CH,
	FN_DFS_RANGE,
	FN_DFS_CHBW,
	FN_COUNTRY,
	FN_INIT_CHANNEL_SET,
};

struct vector {
	char name[MAX_NAME];
	enum chplan_fn fn;
	char obj[4096];
	size_t obj_len;
	int id;
	int ch;
	int hi;
	int lo;
	int bw;
	int offset;
	int expect;
	int expect_chplan;
	int expect_null;
	int wireless_mode;
	int band_cap;
	int expect_count;
	char expect_chset[HOST_VECTOR_MAX_HEX_BUF];
	char alpha2[3];
	u8 excl_chs[MAX_CHANNEL_NUM];
	RT_CHANNEL_INFO chset[MAX_CHANNEL_NUM];
};

static int parse_fn(const char *obj, size_t obj_len, enum chplan_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_chplan_get_default_regd_2g") == 0)
		*out = FN_REGD_2G;
	else if (strcmp(fn, "rtw_chplan_get_default_regd_5g") == 0)
		*out = FN_REGD_5G;
	else if (strcmp(fn, "rtw_chplan_get_default_regd") == 0)
		*out = FN_REGD;
	else if (strcmp(fn, "rtw_chplan_is_empty") == 0)
		*out = FN_IS_EMPTY;
	else if (strcmp(fn, "rtw_is_channel_plan_valid") == 0)
		*out = FN_IS_VALID;
	else if (strcmp(fn, "rtw_regsty_is_excl_chs") == 0)
		*out = FN_EXCL_CHS;
	else if (strcmp(fn, "rtw_chset_is_dfs_ch") == 0)
		*out = FN_DFS_CH;
	else if (strcmp(fn, "rtw_chset_is_dfs_range") == 0)
		*out = FN_DFS_RANGE;
	else if (strcmp(fn, "rtw_chset_is_dfs_chbw") == 0)
		*out = FN_DFS_CHBW;
	else if (strcmp(fn, "rtw_get_chplan_from_country") == 0)
		*out = FN_COUNTRY;
	else if (strcmp(fn, "init_channel_set") == 0)
		*out = FN_INIT_CHANNEL_SET;
	else
		return -1;
	return 0;
}

static int parse_excl_chs(const char *hex, u8 *out)
{
	size_t n = 0;
	size_t len = 0;
	char byte[3];
	size_t i;

	if (!hex || !*hex)
		return 0;

	memset(out, 0, MAX_CHANNEL_NUM);
	for (i = 0; hex[i]; i++) {
		if (hex[i] == ' ' || hex[i] == '\t')
			continue;
		byte[n++] = hex[i];
		if (n == 2) {
			unsigned int val;

			byte[2] = '\0';
			if (sscanf(byte, "%x", &val) != 1)
				return -1;
			if (len >= MAX_CHANNEL_NUM)
				return -1;
			out[len++] = (u8)val;
			n = 0;
		}
	}
	return 0;
}

static int parse_chset_hex(const char *hex, RT_CHANNEL_INFO *out)
{
	size_t n = 0;
	size_t idx = 0;
	char byte[3];
	u8 cur_ch = 0;
	int have_ch = 0;
	size_t i;

	if (!hex || !*hex)
		return 0;

	memset(out, 0, sizeof(RT_CHANNEL_INFO) * MAX_CHANNEL_NUM);
	for (i = 0; hex[i]; i++) {
		if (hex[i] == ' ' || hex[i] == '\t')
			continue;
		byte[n++] = hex[i];
		if (n == 2) {
			unsigned int val;

			byte[2] = '\0';
			if (sscanf(byte, "%x", &val) != 1)
				return -1;
			if (!have_ch) {
				cur_ch = (u8)val;
				have_ch = 1;
			} else {
				if (idx >= MAX_CHANNEL_NUM)
					return -1;
				out[idx].ChannelNum = cur_ch;
				out[idx].flags = (u8)val;
				idx++;
				have_ch = 0;
			}
			n = 0;
		}
	}
	if (n != 0 || have_ch)
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (obj_len >= sizeof(v->obj))
		return -1;
	memcpy(v->obj, obj, obj_len);
	v->obj_len = obj_len;
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "id", &v->id);
	host_json_parse_int_in(obj, obj_len, "ch", &v->ch);
	host_json_parse_int_in(obj, obj_len, "hi", &v->hi);
	host_json_parse_int_in(obj, obj_len, "lo", &v->lo);
	host_json_parse_int_in(obj, obj_len, "bw", &v->bw);
	host_json_parse_int_in(obj, obj_len, "offset", &v->offset);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	host_json_parse_int_in(obj, obj_len, "expect_chplan", &v->expect_chplan);
	host_json_parse_int_in(obj, obj_len, "expect_null", &v->expect_null);
	host_json_parse_int_in(obj, obj_len, "wireless_mode", &v->wireless_mode);
	host_json_parse_int_in(obj, obj_len, "band_cap", &v->band_cap);
	host_json_parse_int_in(obj, obj_len, "expect_count", &v->expect_count);
	if (!host_json_parse_string_in(obj, obj_len, "alpha2", v->alpha2, sizeof(v->alpha2))) {
		if (strlen(v->alpha2) != 2)
			return -1;
	}
	if (!host_json_parse_string_in(obj, obj_len, "excl_chs", hex, sizeof(hex))) {
		if (parse_excl_chs(hex, v->excl_chs))
			return -1;
	}
	if (!host_json_parse_string_in(obj, obj_len, "expect_chset", v->expect_chset,
				       sizeof(v->expect_chset))) {
		if (parse_chset_hex(v->expect_chset, v->chset))
			return -1;
	} else if (!host_json_parse_string_in(obj, obj_len, "chset", hex, sizeof(hex))) {
		if (parse_chset_hex(hex, v->chset))
			return -1;
	}
	return 0;
}

static int chset_matches(const RT_CHANNEL_INFO *got, const RT_CHANNEL_INFO *expect, u8 count)
{
	u8 i;

	for (i = 0; i < count; i++) {
		if (got[i].ChannelNum != expect[i].ChannelNum ||
		    got[i].flags != expect[i].flags)
			return 0;
	}
	return 1;
}

static int run_vector(const struct vector *v)
{
	struct registry_priv regsty;

	memset(&regsty, 0, sizeof(regsty));
	_rtw_memcpy(regsty.excl_chs, v->excl_chs, sizeof(regsty.excl_chs));

	switch (v->fn) {
	case FN_REGD_2G:
		if ((int)rtw_chplan_get_default_regd_2g((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd_2g mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_REGD_5G:
		if ((int)rtw_chplan_get_default_regd_5g((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd_5g mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_REGD:
		if ((int)rtw_chplan_get_default_regd((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_IS_EMPTY:
		if ((int)rtw_chplan_is_empty((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: is_empty mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_IS_VALID:
		if ((int)rtw_is_channel_plan_valid((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: is_valid mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_EXCL_CHS:
		if ((int)rtw_regsty_is_excl_chs(&regsty, (u8)v->ch) != v->expect) {
			fprintf(stderr, "%s: excl_chs mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_DFS_CH:
		if ((int)rtw_chset_is_dfs_ch((RT_CHANNEL_INFO *)v->chset, (u8)v->ch) != v->expect) {
			fprintf(stderr, "%s: dfs_ch mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_DFS_RANGE:
		if ((int)rtw_chset_is_dfs_range((RT_CHANNEL_INFO *)v->chset, (u32)v->hi,
						(u32)v->lo) != v->expect) {
			fprintf(stderr, "%s: dfs_range mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_DFS_CHBW:
		if ((int)rtw_chset_is_dfs_chbw((RT_CHANNEL_INFO *)v->chset, (u8)v->ch,
					       (u8)v->bw, (u8)v->offset) != v->expect) {
			fprintf(stderr, "%s: dfs_chbw mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_COUNTRY: {
		const struct country_chplan *ent;

		ent = rtw_get_chplan_from_country(v->alpha2);
		if (v->expect_null) {
			if (ent != NULL) {
				fprintf(stderr, "%s: expected NULL, got entry\n", v->name);
				return -1;
			}
		} else if (ent == NULL) {
			fprintf(stderr, "%s: expected entry, got NULL\n", v->name);
			return -1;
		} else if ((int)ent->chplan != v->expect_chplan) {
			fprintf(stderr, "%s: chplan mismatch (got %u, expected %d)\n",
				v->name, ent->chplan, v->expect_chplan);
			return -1;
		}
		break;
	}
	case FN_INIT_CHANNEL_SET: {
		_adapter adapter;
		struct rf_ctl_t *rfctl;
		struct registry_priv *regsty;
		u8 count;
		u8 i;

		memset(&adapter, 0, sizeof(adapter));
		rfctl = adapter_to_rfctl(&adapter);
		regsty = adapter_to_regsty(&adapter);
		rfctl->regd_src = REGD_SRC_RTK_PRIV;
		rfctl->ChannelPlan = (u8)v->id;
		regsty->wireless_mode = (u8)v->wireless_mode;
		_rtw_memcpy(regsty->excl_chs, v->excl_chs, sizeof(regsty->excl_chs));
		host_chplan_set_band_cap((u8)v->band_cap);

		count = init_channel_set(&adapter);
		if ((int)count != v->expect_count) {
			fprintf(stderr, "%s: count mismatch (got %u, expected %d)\n",
				v->name, count, v->expect_count);
			return -1;
		}
		if (!chset_matches(rfctl->channel_set, v->chset, count)) {
			fprintf(stderr, "%s: channel_set mismatch\n", v->name);
			for (i = 0; i < count; i++) {
				fprintf(stderr, "  got ch=%u flags=0x%02x expect ch=%u flags=0x%02x\n",
					rfctl->channel_set[i].ChannelNum,
					rfctl->channel_set[i].flags,
					v->chset[i].ChannelNum,
					v->chset[i].flags);
			}
			return -1;
		}
		break;
	}
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "chplan_vectors.json";
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	size_t i;
	size_t executed = 0;
	size_t skipped = 0;
	int failed = 0;

	if (argc > 1)
		path = argv[1];

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to parse %s\n", path);
		return 1;
	}

	for (i = 0; i < nvec; i++) {
#ifndef RUST_CHPLAN_ORACLE
		if (vectors[i].fn == FN_INIT_CHANNEL_SET) {
			printf("skip %s (rust-only after PR3)\n", vectors[i].name);
			skipped++;
			continue;
		}
#endif
		executed++;
		if (run_vector(&vectors[i]) != 0)
			failed++;
		else
			printf("ok %s\n", vectors[i].name);
	}

	if (failed) {
		fprintf(stderr, "%d vector(s) failed\n", failed);
		return 1;
	}
#ifdef RUST_CHPLAN_ORACLE
	if (skipped)
		printf("all %zu chplan vectors passed (%zu skipped; oracle: rust/rtw_chplan.rs)\n",
		       executed, skipped);
	else
		printf("all %zu chplan vectors passed (oracle: rust/rtw_chplan.rs)\n", executed);
#else
	if (skipped)
		printf("all %zu chplan vectors passed (%zu rust-only skipped; oracle: core/rtw_chplan.c)\n",
		       executed, skipped);
	else
		printf("all %zu chplan vectors passed (oracle: core/rtw_chplan.c)\n", executed);
#endif
	return 0;
}
