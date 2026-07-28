// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for chplan_rest beacon-hint helper (W3-17 PR1).
 */

#if defined(HOST_CHPLAN_REST_ORACLE_BUILD) || defined(RUST_CHPLAN_REST_ORACLE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_chplan_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

enum rest_beacon_fn {
	FN_PROCESS_BEACON_HINT = 0,
};

struct vector {
	char name[MAX_NAME];
	enum rest_beacon_fn fn;
	u8 ch;
	RT_CHANNEL_INFO chset[MAX_CHANNEL_NUM];
	char alpha2[3];
	int has_country_ent;
	struct country_chplan country;
	int expect_act_cnt;
	RT_CHANNEL_INFO expect_chset[MAX_CHANNEL_NUM];
};

#ifdef RUST_CHPLAN_REST_ORACLE
extern u8 host_rest_process_beacon_hint(_adapter *adapter, WLAN_BSSID_EX *bss);
#endif

static int parse_fn(const char *obj, size_t obj_len, enum rest_beacon_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "host_rest_process_beacon_hint") == 0)
		*out = FN_PROCESS_BEACON_HINT;
	else
		return -1;
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

static int chset_equal(const RT_CHANNEL_INFO *a, const RT_CHANNEL_INFO *b)
{
	size_t i;

	for (i = 0; i < MAX_CHANNEL_NUM; i++) {
		if (a[i].ChannelNum != b[i].ChannelNum || a[i].flags != b[i].flags)
			return 0;
		if (a[i].ChannelNum == 0 && b[i].ChannelNum == 0)
			break;
	}
	return 1;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char chset_hex[HOST_VECTOR_MAX_HEX_BUF];
	char expect_hex[HOST_VECTOR_MAX_HEX_BUF];
	int country_ent = 1;
	int ch = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	if (!host_json_parse_int_in(obj, obj_len, "ch", &ch))
		v->ch = (u8)ch;
	else
		return -1;
	if (host_json_parse_string_in(obj, obj_len, "chset", chset_hex, sizeof(chset_hex)))
		return -1;
	if (parse_chset_hex(chset_hex, v->chset))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "expect_act_cnt", &v->expect_act_cnt))
		v->expect_act_cnt = 0;
	if (host_json_parse_string_in(obj, obj_len, "expect_chset", expect_hex,
				      sizeof(expect_hex)))
		return -1;
	if (parse_chset_hex(expect_hex, v->expect_chset))
		return -1;
	if (!host_json_parse_string_in(obj, obj_len, "alpha2", v->alpha2, sizeof(v->alpha2))) {
		v->country.alpha2[0] = (u8)v->alpha2[0];
		v->country.alpha2[1] = (u8)v->alpha2[1];
	}
	if (!host_json_parse_int_in(obj, obj_len, "country_ent", &country_ent))
		v->has_country_ent = country_ent;
	else
		v->has_country_ent = 1;
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	WLAN_BSSID_EX bss;
	u8 act_cnt;

	memset(&adapter, 0, sizeof(adapter));
	memset(&bss, 0, sizeof(bss));
	memcpy(adapter.rf_ctl.channel_set, v->chset, sizeof(v->chset));
	if (v->has_country_ent)
		adapter.rf_ctl.country_ent = &v->country;
	bss.Configuration.DSConfig = v->ch;

	act_cnt = host_rest_process_beacon_hint(&adapter, &bss);
	if (act_cnt != (u8)v->expect_act_cnt) {
		fprintf(stderr, "%s: act_cnt got %u expect %d\n", v->name, act_cnt,
			v->expect_act_cnt);
		return -1;
	}
	if (!chset_equal(adapter.rf_ctl.channel_set, v->expect_chset)) {
		fprintf(stderr, "%s: chset mismatch after beacon hint\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "chplan_rest_vectors.json";
	static struct vector vectors[MAX_VECTORS];
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
	printf("all %zu rest beacon-hint vectors passed\n", nvec);
	return 0;
}

#endif /* HOST_CHPLAN_REST_ORACLE_BUILD || RUST_CHPLAN_REST_ORACLE */
