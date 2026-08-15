// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for rtw_check_for_vht20 (W3-45). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vht_mcs_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128
#define MAX_IE 256

struct vector {
	char name[MAX_NAME];
	u8 ies[MAX_IE];
	size_t ies_len;
	u8 expect_width;
	u8 expect_center1;
	u8 expect_center2;
	int expect_found;
};

extern _adapter host_vht_mcs_adapter;

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, obj_len, "ies", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->ies, sizeof(v->ies), &decoded))
		return -1;
	v->ies_len = decoded;
	if (host_json_parse_int_in(obj, obj_len, "expect_vht_width", &tmp))
		return -1;
	v->expect_width = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_center1", &tmp))
		return -1;
	v->expect_center1 = (u8)tmp;
	if (host_json_parse_int_in(obj, obj_len, "expect_center2", &tmp))
		return -1;
	v->expect_center2 = (u8)tmp;
	return host_json_parse_int_in(obj, obj_len, "expect_found", &v->expect_found);
}

static int run_vector(struct vector *v)
{
	u8 *vht_op_ie;
	int vht_op_ielen;
	u8 width, c1, c2;

	rtw_check_for_vht20(&host_vht_mcs_adapter, v->ies, (int)v->ies_len);
	vht_op_ie = rtw_get_ie(v->ies, EID_VHTOperation, &vht_op_ielen, (int)v->ies_len);
	if (v->expect_found) {
		if (!vht_op_ie || !vht_op_ielen) {
			fprintf(stderr, "%s: expected VHT op IE\n", v->name);
			return -1;
		}
		width = GET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2);
		c1 = *(vht_op_ie + 3);
		c2 = *(vht_op_ie + 4);
		if (width != v->expect_width || c1 != v->expect_center1 ||
		    c2 != v->expect_center2) {
			fprintf(stderr,
				"%s: got width=%u c1=%u c2=%u expected %u/%u/%u\n",
				v->name, width, c1, c2, v->expect_width,
				v->expect_center1, v->expect_center2);
			return -1;
		}
		return 0;
	}
	if (vht_op_ie && vht_op_ielen) {
		width = GET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2);
		if (width != v->expect_width) {
			fprintf(stderr, "%s: unexpected width change to %u\n",
				v->name, width);
			return -1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "vht_vht20_vectors.json";

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
