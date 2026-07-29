// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_rf_rest channel layout + freq + op-class helpers
 * (W3-19, W3-20, W3-22).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 160
#define MAX_NAME 128
#define MAX_OP_CHS 8

enum rf_fn {
	FN_SCCH_OFFSET = 0,
	FN_SCCH_OPCH,
	FN_CENTER_CHS_2G_NUM,
	FN_CENTER_CHS_2G,
	FN_CENTER_CHS_5G_NUM,
	FN_CENTER_CHS_5G,
	FN_OP_CHS,
	FN_OFFSET_BY_CHBW,
	FN_CENTER_CH,
	FN_CH_GROUP,
	FN_CH2FREQ,
	FN_FREQ2CH,
	FN_CHBW_TO_FREQ_RANGE,
	FN_CH_WIDTH_STR,
	FN_CH_WIDTH_TO_BW_CAP,
	FN_BAND_STR,
	FN_BAND_TO_BAND_CAP,
	FN_OPC_BW_STR,
	FN_OPC_BW_TO_CH_WIDTH,
	FN_VALID_GLOBAL_OP_CLASS_ID,
	FN_GET_SUB_OP_CLASS,
	FN_GET_OP_CLASS_BY_CHBW,
	FN_GET_BW_OFFSET_BY_OP_CLASS_CH,
};

struct vector {
	char name[MAX_NAME];
	enum rf_fn fn;
	u8 cch;
	u8 bw;
	u8 offset;
	u8 opch;
	u8 ch;
	u8 id;
	u8 r_offset_in;
	int expect;
	int expect_valid;
	int expect_op_ch_num;
	u8 expect_op_chs[MAX_OP_CHS];
	int expect_band;
	int expect_group;
	int expect_cck_group;
	int has_cck_group;
	int freq;
	int expect_hi;
	int expect_lo;
	int expect_offset;
	char expect_str[MAX_NAME];
};

u8 rtw_get_scch_by_cch_offset(u8 cch, u8 bw, u8 offset);
u8 rtw_get_scch_by_cch_opch(u8 cch, u8 bw, u8 opch);
u8 center_chs_2g_num(u8 bw);
u8 center_chs_2g(u8 bw, u8 id);
u8 center_chs_5g_num(u8 bw);
u8 center_chs_5g(u8 bw, u8 id);
u8 rtw_get_op_chs_by_cch_bw(u8 cch, u8 bw, u8 **op_chs, u8 *op_ch_num);
u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);
u8 rtw_get_ch_group(u8 ch, u8 *group, u8 *cck_group);
int rtw_ch2freq(int chan);
int rtw_freq2ch(int freq);
bool rtw_chbw_to_freq_range(u8 ch, u8 bw, u8 offset, u32 *hi, u32 *lo);

bool is_valid_global_op_class_id(u8 gid);
s16 get_sub_op_class(u8 gid, u8 ch);
u8 rtw_get_op_class_by_chbw(u8 ch, u8 bw, u8 offset);
u8 rtw_get_bw_offset_by_op_class_ch(u8 gid, u8 ch, u8 *bw, u8 *offset);

static int parse_fn(const char *obj, size_t obj_len, enum rf_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_get_scch_by_cch_offset") == 0)
		*out = FN_SCCH_OFFSET;
	else if (strcmp(fn, "rtw_get_scch_by_cch_opch") == 0)
		*out = FN_SCCH_OPCH;
	else if (strcmp(fn, "center_chs_2g_num") == 0)
		*out = FN_CENTER_CHS_2G_NUM;
	else if (strcmp(fn, "center_chs_2g") == 0)
		*out = FN_CENTER_CHS_2G;
	else if (strcmp(fn, "center_chs_5g_num") == 0)
		*out = FN_CENTER_CHS_5G_NUM;
	else if (strcmp(fn, "center_chs_5g") == 0)
		*out = FN_CENTER_CHS_5G;
	else if (strcmp(fn, "rtw_get_op_chs_by_cch_bw") == 0)
		*out = FN_OP_CHS;
	else if (strcmp(fn, "rtw_get_offset_by_chbw") == 0)
		*out = FN_OFFSET_BY_CHBW;
	else if (strcmp(fn, "rtw_get_center_ch") == 0)
		*out = FN_CENTER_CH;
	else if (strcmp(fn, "rtw_get_ch_group") == 0)
		*out = FN_CH_GROUP;
	else if (strcmp(fn, "rtw_ch2freq") == 0)
		*out = FN_CH2FREQ;
	else if (strcmp(fn, "rtw_freq2ch") == 0)
		*out = FN_FREQ2CH;
	else if (strcmp(fn, "rtw_chbw_to_freq_range") == 0)
		*out = FN_CHBW_TO_FREQ_RANGE;
	else if (strcmp(fn, "ch_width_str") == 0)
		*out = FN_CH_WIDTH_STR;
	else if (strcmp(fn, "ch_width_to_bw_cap") == 0)
		*out = FN_CH_WIDTH_TO_BW_CAP;
	else if (strcmp(fn, "band_str") == 0)
		*out = FN_BAND_STR;
	else if (strcmp(fn, "band_to_band_cap") == 0)
		*out = FN_BAND_TO_BAND_CAP;
	else if (strcmp(fn, "opc_bw_str") == 0)
		*out = FN_OPC_BW_STR;
	else if (strcmp(fn, "opc_bw_to_ch_width") == 0)
		*out = FN_OPC_BW_TO_CH_WIDTH;
	else if (strcmp(fn, "is_valid_global_op_class_id") == 0)
		*out = FN_VALID_GLOBAL_OP_CLASS_ID;
	else if (strcmp(fn, "get_sub_op_class") == 0)
		*out = FN_GET_SUB_OP_CLASS;
	else if (strcmp(fn, "rtw_get_op_class_by_chbw") == 0)
		*out = FN_GET_OP_CLASS_BY_CHBW;
	else if (strcmp(fn, "rtw_get_bw_offset_by_op_class_ch") == 0)
		*out = FN_GET_BW_OFFSET_BY_OP_CLASS_CH;
	else
		return -1;
	return 0;
}

static int parse_u8_array_in(const char *obj, size_t obj_len, const char *key,
			     u8 *out, size_t out_cap, size_t *count_out)
{
	const char *p = host_json_find_key_in(obj, obj_len, key);
	size_t n = 0;

	if (!p || *p != '[')
		return -1;
	p++;
	while (n < out_cap) {
		int val;

		p = host_json_skip_ws(p);
		if (*p == ']') {
			p++;
			*count_out = n;
			return 0;
		}
		if (n > 0) {
			if (*p != ',')
				return -1;
			p++;
			p = host_json_skip_ws(p);
		}
		val = 0;
		if (*p == '-')
			return -1;
		while (*p >= '0' && *p <= '9') {
			val = val * 10 + (*p - '0');
			p++;
		}
		out[n++] = (u8)val;
	}
	return -1;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	size_t op_n = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "cch", (int *)&v->cch);
	host_json_parse_int_in(obj, obj_len, "bw", (int *)&v->bw);
	host_json_parse_int_in(obj, obj_len, "offset", (int *)&v->offset);
	host_json_parse_int_in(obj, obj_len, "opch", (int *)&v->opch);
	host_json_parse_int_in(obj, obj_len, "ch", (int *)&v->ch);
	host_json_parse_int_in(obj, obj_len, "freq", &v->freq);
	host_json_parse_int_in(obj, obj_len, "id", (int *)&v->id);
	host_json_parse_int_in(obj, obj_len, "gid", (int *)&v->id);
	host_json_parse_int_in(obj, obj_len, "r_offset_in", (int *)&v->r_offset_in);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	host_json_parse_int_in(obj, obj_len, "expect_valid", &v->expect_valid);
	host_json_parse_int_in(obj, obj_len, "expect_op_ch_num", &v->expect_op_ch_num);
	host_json_parse_int_in(obj, obj_len, "expect_band", &v->expect_band);
	host_json_parse_int_in(obj, obj_len, "expect_group", &v->expect_group);
	host_json_parse_int_in(obj, obj_len, "expect_hi", &v->expect_hi);
	host_json_parse_int_in(obj, obj_len, "expect_lo", &v->expect_lo);
	host_json_parse_int_in(obj, obj_len, "expect_offset", &v->expect_offset);
	host_json_parse_string_in(obj, obj_len, "expect_str", v->expect_str,
				  sizeof(v->expect_str));
	if (!host_json_parse_int_in(obj, obj_len, "expect_cck_group", &tmp)) {
		v->expect_cck_group = tmp;
		v->has_cck_group = 1;
	}
	if (!parse_u8_array_in(obj, obj_len, "expect_op_chs", v->expect_op_chs,
			       MAX_OP_CHS, &op_n))
		(void)op_n;
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_SCCH_OFFSET: {
		u8 got = rtw_get_scch_by_cch_offset(v->cch, v->bw, v->offset);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: scch_offset got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_SCCH_OPCH: {
		u8 got = rtw_get_scch_by_cch_opch(v->cch, v->bw, v->opch);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: scch_opch got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CENTER_CHS_2G_NUM: {
		u8 got = center_chs_2g_num(v->bw);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: center_chs_2g_num got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CENTER_CHS_2G: {
		u8 got = center_chs_2g(v->bw, v->id);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: center_chs_2g got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CENTER_CHS_5G_NUM: {
		u8 got = center_chs_5g_num(v->bw);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: center_chs_5g_num got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CENTER_CHS_5G: {
		u8 got = center_chs_5g(v->bw, v->id);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: center_chs_5g got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_OP_CHS: {
		u8 *op_chs = NULL;
		u8 op_ch_num = 0;
		u8 valid = rtw_get_op_chs_by_cch_bw(v->cch, v->bw, &op_chs, &op_ch_num);
		size_t i;

		if ((int)valid != v->expect_valid) {
			fprintf(stderr, "%s: op_chs valid=%u expect=%d\n",
				v->name, valid, v->expect_valid);
			return -1;
		}
		if (!valid)
			break;
		if ((int)op_ch_num != v->expect_op_ch_num) {
			fprintf(stderr, "%s: op_ch_num=%u expect=%d\n",
				v->name, op_ch_num, v->expect_op_ch_num);
			return -1;
		}
		for (i = 0; i < op_ch_num; i++) {
			if (op_chs[i] != v->expect_op_chs[i]) {
				fprintf(stderr,
					"%s: op_chs[%zu]=%u expect=%u\n",
					v->name, i, op_chs[i], v->expect_op_chs[i]);
				return -1;
			}
		}
		break;
	}
	case FN_OFFSET_BY_CHBW: {
		u8 r_offset = v->r_offset_in;
		u8 valid = rtw_get_offset_by_chbw(v->ch, v->bw, &r_offset);

		if ((int)valid != v->expect_valid) {
			fprintf(stderr, "%s: offset valid=%u expect=%d\n",
				v->name, valid, v->expect_valid);
			return -1;
		}
		if (valid && (int)r_offset != v->expect) {
			fprintf(stderr, "%s: offset=%u expect=%d\n",
				v->name, r_offset, v->expect);
			return -1;
		}
		break;
	}
	case FN_CENTER_CH: {
		u8 got = rtw_get_center_ch(v->ch, v->bw, v->offset);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: center_ch got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CH_GROUP: {
		u8 group = 0xff;
		u8 cck_group = 0xff;
		u8 band = rtw_get_ch_group(v->ch, &group, &cck_group);

		if ((int)band != v->expect_band) {
			fprintf(stderr, "%s: band=%u expect=%d\n",
				v->name, band, v->expect_band);
			return -1;
		}
		if ((int)group != v->expect_group) {
			fprintf(stderr, "%s: group=%u expect=%d\n",
				v->name, group, v->expect_group);
			return -1;
		}
		if (v->has_cck_group && (int)cck_group != v->expect_cck_group) {
			fprintf(stderr, "%s: cck_group=%u expect=%d\n",
				v->name, cck_group, v->expect_cck_group);
			return -1;
		}
		break;
	}
	case FN_CH2FREQ: {
		int got = rtw_ch2freq(v->ch);

		if (got != v->expect) {
			fprintf(stderr, "%s: ch2freq got=%d expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_FREQ2CH: {
		int got = rtw_freq2ch(v->freq);

		if (got != v->expect) {
			fprintf(stderr, "%s: freq2ch got=%d expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_CHBW_TO_FREQ_RANGE: {
		u32 hi = 0, lo = 0;
		bool valid = rtw_chbw_to_freq_range(v->ch, v->bw, v->offset, &hi, &lo);

		if ((int)valid != v->expect_valid) {
			fprintf(stderr, "%s: chbw valid=%d expect=%d\n",
				v->name, valid, v->expect_valid);
			return -1;
		}
		if (!valid)
			break;
		if ((int)hi != v->expect_hi) {
			fprintf(stderr, "%s: hi=%u expect=%d\n",
				v->name, hi, v->expect_hi);
			return -1;
		}
		if ((int)lo != v->expect_lo) {
			fprintf(stderr, "%s: lo=%u expect=%d\n",
				v->name, lo, v->expect_lo);
			return -1;
		}
		break;
	}
	case FN_CH_WIDTH_STR: {
		const char *got = ch_width_str(v->bw);

		if (strcmp(got, v->expect_str) != 0) {
			fprintf(stderr, "%s: ch_width_str got=%s expect=%s\n",
				v->name, got, v->expect_str);
			return -1;
		}
		break;
	}
	case FN_CH_WIDTH_TO_BW_CAP: {
		u8 got = ch_width_to_bw_cap(v->bw);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: ch_width_to_bw_cap got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_BAND_STR: {
		const char *got = band_str(v->bw);

		if (strcmp(got, v->expect_str) != 0) {
			fprintf(stderr, "%s: band_str got=%s expect=%s\n",
				v->name, got, v->expect_str);
			return -1;
		}
		break;
	}
	case FN_BAND_TO_BAND_CAP: {
		u8 got = band_to_band_cap(v->bw);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: band_to_band_cap got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_OPC_BW_STR: {
		const char *got = opc_bw_str(v->bw);

		if (strcmp(got, v->expect_str) != 0) {
			fprintf(stderr, "%s: opc_bw_str got=%s expect=%s\n",
				v->name, got, v->expect_str);
			return -1;
		}
		break;
	}
	case FN_OPC_BW_TO_CH_WIDTH: {
		u8 got = opc_bw_to_ch_width(v->bw);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: opc_bw_to_ch_width got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_VALID_GLOBAL_OP_CLASS_ID: {
		bool got = is_valid_global_op_class_id(v->id);

		if ((int)got != v->expect_valid) {
			fprintf(stderr, "%s: is_valid_global_op_class_id got=%d expect=%d\n",
				v->name, got, v->expect_valid);
			return -1;
		}
		break;
	}
	case FN_GET_SUB_OP_CLASS: {
		s16 got = get_sub_op_class(v->id, v->ch);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: get_sub_op_class got=%d expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_GET_OP_CLASS_BY_CHBW: {
		u8 got = rtw_get_op_class_by_chbw(v->ch, v->bw, v->offset);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: rtw_get_op_class_by_chbw got=%u expect=%d\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_GET_BW_OFFSET_BY_OP_CLASS_CH: {
		u8 out_bw = 0;
		u8 out_offset = 0;
		u8 valid = rtw_get_bw_offset_by_op_class_ch(v->id, v->ch, &out_bw,
							    &out_offset);

		if ((int)valid != v->expect_valid) {
			fprintf(stderr, "%s: rtw_get_bw_offset_by_op_class_ch valid=%u expect=%d\n",
				v->name, valid, v->expect_valid);
			return -1;
		}
		if (!valid)
			break;
		if ((int)out_bw != v->expect) {
			fprintf(stderr, "%s: bw=%u expect=%d\n",
				v->name, out_bw, v->expect);
			return -1;
		}
		if ((int)out_offset != v->expect_offset) {
			fprintf(stderr, "%s: offset=%u expect=%d\n",
				v->name, out_offset, v->expect_offset);
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
	struct vector vecs[MAX_VECTORS];
	size_t count = 0;
	size_t executed = 0;
	size_t i;
	const char *path = "rf_vectors.json";

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
#ifdef RUST_RF_REST_ORACLE
	printf("PASS %zu vectors (oracle: rust/rtw_rf_rest.rs) (%s)\n",
	       executed, path);
#else
	printf("PASS %zu vectors (%s)\n", executed, path);
#endif
	return 0;
}
