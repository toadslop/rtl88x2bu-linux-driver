// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_restructure_vht_ie (W3-36). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vht_rest.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_IE 256

struct vector {
	char name[MAX_NAME];
	u8 in_ie[MAX_IE];
	size_t in_len;
	u8 hal_max_bw;
	u8 chset_max_bw;
	u8 regsty_bw;
	u8 expect_vht_option;
	u8 expect_vht_op_width;
	u8 expect_vht_op_cfreq1;
};

#ifndef RUST_VHT_ORACLE
u32 rtw_restructure_vht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie, uint in_len,
			   uint *pout_len);
#else
u32 rtw_restructure_vht_ie(_adapter *padapter, u8 *in_ie, u8 *out_ie, unsigned in_len,
			   unsigned *pout_len);
#endif

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	memset(v, 0, sizeof(*v));
	v->hal_max_bw = CHANNEL_WIDTH_80;
	v->chset_max_bw = CHANNEL_WIDTH_80;
	v->regsty_bw = CHANNEL_WIDTH_80;
	v->expect_vht_option = 1;

	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "in_ie_hex", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->in_ie, sizeof(v->in_ie), &decoded))
		return -1;
	v->in_len = decoded;
	host_json_parse_int_in(obj, len, "hal_max_bw", (int *)&v->hal_max_bw);
	host_json_parse_int_in(obj, len, "chset_max_bw", (int *)&v->chset_max_bw);
	host_json_parse_int_in(obj, len, "regsty_bw", (int *)&v->regsty_bw);
	host_json_parse_int_in(obj, len, "expect_vht_option", (int *)&v->expect_vht_option);
	host_json_parse_int_in(obj, len, "expect_vht_op_width", (int *)&v->expect_vht_op_width);
	host_json_parse_int_in(obj, len, "expect_vht_op_cfreq1", (int *)&v->expect_vht_op_cfreq1);
	return 0;
}

static int run_vector(const struct vector *v)
{
	_adapter adapter;
	u8 out_ie[MAX_IE];
	uint out_len = 0;
	uint len = 0;
	const u8 *vht_op;

	memset(&adapter, 0, sizeof(adapter));
	adapter.host_fixture.hal_max_bw = v->hal_max_bw;
	adapter.host_fixture.chset_max_bw = v->chset_max_bw;
	adapter.registrypriv.bw_mode = v->regsty_bw;
	host_vht_rest_adapter = &adapter;

	if (rtw_restructure_vht_ie(&adapter, (u8 *)v->in_ie, out_ie, (uint)v->in_len,
				   &out_len) != v->expect_vht_option) {
		fprintf(stderr, "%s: vht_option mismatch\n", v->name);
		return -1;
	}
	if (!v->expect_vht_option)
		return 0;

	vht_op = rtw_get_ie(out_ie, EID_VHTOperation, &len, out_len);
	if (!vht_op || len != VHT_OP_IE_LEN ||
	    GET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op + 2) != v->expect_vht_op_width ||
	    vht_op[3] != v->expect_vht_op_cfreq1) {
		fprintf(stderr, "%s: vht_op mismatch\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "vht_restructure_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &count)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (i = 0; i < count; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "FAIL: %s\n", vectors[i].name);
			return 1;
		}
	}
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
