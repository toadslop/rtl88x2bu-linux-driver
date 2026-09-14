// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle for rtw_build_vht_operation_ie (W3-84). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vht_build_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_IE 32

struct vector {
	char name[MAX_NAME];
	u8 reg_bw_mode;
	u8 hal_bw_cap;
	u8 mcs_map[2];
	u8 channel;
	u8 expect_width;
	u8 expect_center1;
	u8 expect_center2;
	u8 expect_mcs0;
	u8 expect_mcs1;
};

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "reg_bw_mode", &tmp))
		return -1;
	v->reg_bw_mode = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "hal_bw_cap", &tmp))
		return -1;
	v->hal_bw_cap = (u8)tmp;
	if (host_json_parse_string_in(obj, obj_len, "mcs_map", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->mcs_map, sizeof(v->mcs_map), &decoded) || decoded != 2)
		return -1;
	if (host_json_parse_int_in(obj, obj_len, "channel", &tmp))
		return -1;
	v->channel = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_width", &tmp))
		return -1;
	v->expect_width = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_center1", &tmp))
		return -1;
	v->expect_center1 = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_center2", &tmp))
		return -1;
	v->expect_center2 = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_mcs0", &tmp))
		return -1;
	v->expect_mcs0 = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_mcs1", &tmp))
		return -1;
	v->expect_mcs1 = (u8)tmp;
	return 0;
}

static int run_vector(struct vector *v)
{
	u8 out[MAX_IE];
	u32 len;
	const u8 *body;

	memset(&host_vht_build_adapter, 0, sizeof(host_vht_build_adapter));
	host_vht_build_adapter.registrypriv.bw_mode = v->reg_bw_mode;
	host_vht_build_hal_bw_cap = v->hal_bw_cap;
	memcpy(host_vht_build_adapter.mlmepriv.vhtpriv.vht_mcs_map, v->mcs_map, 2);

	len = rtw_build_vht_operation_ie(&host_vht_build_adapter, out, v->channel);
	body = out + 2;
	if (len != 7 || out[0] != EID_VHTOperation || out[1] != 5 ||
	    GET_VHT_OPERATION_ELE_CHL_WIDTH(body) != v->expect_width ||
	    body[1] != v->expect_center1 || body[2] != v->expect_center2 ||
	    body[3] != v->expect_mcs0 || body[4] != v->expect_mcs1) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0;
	size_t i;
	size_t failures = 0;

	if (argc != 2 ||
	    host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&vectors[i]))
			failures++;
	printf("%zu vectors, %zu failures\n", n, failures);
	return failures ? 1 : 0;
}
