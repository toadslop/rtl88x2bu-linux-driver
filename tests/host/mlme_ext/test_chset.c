// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for mlme_ext chset helpers (W3-54).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_mlme_ext_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128
#define MAX_CHSET 8

enum chset_fn {
	FN_SEARCH = 0,
	FN_VALID,
	FN_SYNC,
	FN_NON_OCP,
	FN_UPDATE_NON_OCP,
};

struct vector {
	char name[MAX_NAME];
	enum chset_fn fn;
	u8 chset[MAX_CHSET];
	u8 flags[MAX_CHSET];
	u32 ch;
	u8 bw;
	u8 offset;
	u8 req_ch;
	u8 req_bw;
	u8 req_offset;
	u8 g_ch;
	u8 g_bw;
	u8 g_offset;
	u8 allow_primary_passive;
	u8 allow_passive;
	s32 current_time_ms;
	s32 non_ocp_end_ms[MAX_CHSET];
	int update_ms;
	int expect_idx;
	int expect_valid;
	int expect_non_ocp;
	u32 expect_non_ocp_ms;
	int expect_updated;
	u8 expect_req_ch;
	u8 expect_req_bw;
	u8 expect_req_offset;
	u8 expect_g_bw;
};

static RT_CHANNEL_INFO chset_buf[MAX_CHANNEL_NUM];

static void reset_chset(void)
{
	memset(chset_buf, 0, sizeof(chset_buf));
	host_mlme_ext_set_current_time(0);
}

static void load_chset(struct vector *v)
{
	size_t i;

	reset_chset();
	for (i = 0; i < MAX_CHSET && v->chset[i]; i++) {
		chset_buf[i].ChannelNum = v->chset[i];
		chset_buf[i].flags = v->flags[i];
		if (v->non_ocp_end_ms[i] > 0)
			chset_buf[i].non_ocp_end_time =
				(systime)v->non_ocp_end_ms[i];
	}
	host_mlme_ext_set_current_time((systime)v->current_time_ms);
}

static int parse_fn(const char *obj, size_t len, enum chset_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "search"))
		*out = FN_SEARCH;
	else if (!strcmp(fn, "valid"))
		*out = FN_VALID;
	else if (!strcmp(fn, "sync"))
		*out = FN_SYNC;
	else if (!strcmp(fn, "non_ocp"))
		*out = FN_NON_OCP;
	else if (!strcmp(fn, "update_non_ocp"))
		*out = FN_UPDATE_NON_OCP;
	else
		return -1;
	return 0;
}

static int parse_u8_array(const char *obj, size_t len, const char *key,
			    u8 *out, size_t out_len)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	size_t i;

	memset(out, 0, out_len);
	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return 0;
	if (host_hex_decode(hex, (unsigned char *)out, out_len, &decoded))
		return -1;
	for (i = decoded; i < out_len; i++)
		out[i] = 0;
	return 0;
}

static int parse_s32_array(const char *obj, size_t len, const char *key,
			   s32 *out, size_t out_len)
{
	char buf[128];
	const char *p, *end;
	size_t i = 0;

	memset(out, 0, out_len * sizeof(s32));
	if (host_json_parse_string_in(obj, len, key, buf, sizeof(buf)))
		return 0;
	p = buf;
	end = buf + strlen(buf);
	while (p < end && i < out_len) {
		out[i++] = (s32)strtol(p, (char **)&p, 10);
		while (p < end && (*p == ',' || *p == ' '))
			p++;
	}
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
	if (parse_u8_array(obj, len, "chset", v->chset, MAX_CHSET))
		return -1;
	if (parse_u8_array(obj, len, "flags", v->flags, MAX_CHSET))
		return -1;
	if (parse_s32_array(obj, len, "non_ocp_end_ms", v->non_ocp_end_ms,
			    MAX_CHSET))
		return -1;
	if (!host_json_parse_int_in(obj, len, "ch", &tmp))
		v->ch = (u32)tmp;
	if (!host_json_parse_int_in(obj, len, "bw", &tmp))
		v->bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "offset", &tmp))
		v->offset = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "req_ch", &tmp))
		v->req_ch = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "req_bw", &tmp))
		v->req_bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "req_offset", &tmp))
		v->req_offset = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "g_ch", &tmp))
		v->g_ch = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "g_bw", &tmp))
		v->g_bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "g_offset", &tmp))
		v->g_offset = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "allow_primary_passive", &tmp))
		v->allow_primary_passive = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "allow_passive", &tmp))
		v->allow_passive = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "current_time_ms", &tmp))
		v->current_time_ms = (s32)tmp;
	if (!host_json_parse_int_in(obj, len, "update_ms", &tmp))
		v->update_ms = tmp;
	host_json_parse_int_in(obj, len, "expect_idx", &v->expect_idx);
	host_json_parse_int_in(obj, len, "expect_valid", &v->expect_valid);
	host_json_parse_int_in(obj, len, "expect_non_ocp", &v->expect_non_ocp);
	if (!host_json_parse_int_in(obj, len, "expect_non_ocp_ms", &tmp))
		v->expect_non_ocp_ms = (u32)tmp;
	host_json_parse_int_in(obj, len, "expect_updated", &v->expect_updated);
	if (!host_json_parse_int_in(obj, len, "expect_req_ch", &tmp))
		v->expect_req_ch = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_req_bw", &tmp))
		v->expect_req_bw = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_req_offset", &tmp))
		v->expect_req_offset = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_g_bw", &tmp))
		v->expect_g_bw = (u8)tmp;
	return 0;
}

static int run_vector(struct vector *v)
{
	load_chset(v);

	switch (v->fn) {
	case FN_SEARCH: {
		int idx = rtw_chset_search_ch(chset_buf, v->ch);

		if (idx != v->expect_idx) {
			fprintf(stderr, "%s: search(%u) -> %d expected %d\n",
				v->name, v->ch, idx, v->expect_idx);
			return -1;
		}
		break;
	}
	case FN_VALID: {
		u8 valid = rtw_chset_is_chbw_valid(chset_buf, v->ch, v->bw,
						   v->offset,
						   v->allow_primary_passive,
						   v->allow_passive);

		if (valid != (u8)v->expect_valid) {
			fprintf(stderr,
				"%s: valid(%u,%u,%u) -> %u expected %d\n",
				v->name, v->ch, v->bw, v->offset, valid,
				v->expect_valid);
			return -1;
		}
		break;
	}
	case FN_SYNC: {
		u8 req_ch = v->req_ch, req_bw = v->req_bw, req_offset = v->req_offset;
		u8 g_ch = v->g_ch, g_bw = v->g_bw, g_offset = v->g_offset;

		rtw_chset_sync_chbw(chset_buf, &req_ch, &req_bw, &req_offset,
				    &g_ch, &g_bw, &g_offset,
				    v->allow_primary_passive, v->allow_passive);
		if (req_ch != v->expect_req_ch || req_bw != v->expect_req_bw ||
		    req_offset != v->expect_req_offset ||
		    g_bw != v->expect_g_bw) {
			fprintf(stderr,
				"%s: sync -> req %u,%u,%u g_bw %u expected req %u,%u,%u g_bw %u\n",
				v->name, req_ch, req_bw, req_offset, g_bw,
				v->expect_req_ch, v->expect_req_bw,
				v->expect_req_offset, v->expect_g_bw);
			return -1;
		}
		break;
	}
#ifdef CONFIG_DFS_MASTER
	case FN_NON_OCP: {
		bool non_ocp = rtw_chset_is_chbw_non_ocp(chset_buf, v->ch,
							 v->bw, v->offset);
		u32 ms = rtw_chset_get_ch_non_ocp_ms(chset_buf, v->ch, v->bw,
						     v->offset);

		if (non_ocp != (bool)v->expect_non_ocp ||
		    ms != v->expect_non_ocp_ms) {
			fprintf(stderr,
				"%s: non_ocp -> %d ms %u expected %d ms %u\n",
				v->name, non_ocp, ms, v->expect_non_ocp,
				v->expect_non_ocp_ms);
			return -1;
		}
		break;
	}
	case FN_UPDATE_NON_OCP: {
		bool updated = rtw_chset_update_non_ocp_ms(
			chset_buf, v->ch, v->bw, v->offset, v->update_ms);

		if (updated != (bool)v->expect_updated) {
			fprintf(stderr, "%s: update -> %d expected %d\n",
				v->name, updated, v->expect_updated);
			return -1;
		}
		break;
	}
#endif
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec;
	size_t i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <vectors.json>\n", argv[0]);
		return 2;
	}
	if (host_load_vectors(argv[1], vectors, sizeof(struct vector),
			      MAX_VECTORS, parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to load %s\n", argv[1]);
		return 2;
	}
	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL at vector %zu (%s)\n", i,
				vectors[i].name);
			return 1;
		}
	}
	printf("PASS %zu vectors\n", nvec);
	return 0;
}
