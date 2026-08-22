// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-55 AP TIM/VAPID helpers. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ap_rest_types.h"
#include "host_vector_json.h"

static void host_ap_rest_set_vap_map(struct dvobj_priv *dvobj, u8 vap_map)
{
	dvobj->vap_map = vap_map;
}

static u8 host_ap_rest_get_vap_map(struct dvobj_priv *dvobj)
{
	return dvobj->vap_map;
}

#define MAX_VECTORS 12
#define MAX_NAME 128
#define MAX_TIM_BMP 8
#define MAX_IE 32

struct vector {
	char name[MAX_NAME];
	char fn[32];
	u8 dtim_cnt, dtim_period, tim_bmp[MAX_TIM_BMP], tim_bmp_len;
	u8 vap_map, vap_id, expect_vap_id, expect_vap_map;
	int expect_len, expect_ret;
	char expect_ie[64];
};

static int parse_hex(const char *hex, u8 *out, size_t cap, size_t *len)
{
	size_t n = strlen(hex), i;

	if (n % 2)
		return -1;
	*len = n / 2;
	if (*len > cap)
		return -1;
	for (i = 0; i < *len; i++) {
		unsigned v;
		if (sscanf(hex + i * 2, "%2x", &v) != 1)
			return -1;
		out[i] = (u8)v;
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "dtim_cnt", &tmp))
		v->dtim_cnt = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "dtim_period", &tmp))
		v->dtim_period = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "tim_bmp_len", &tmp))
		v->tim_bmp_len = (u8)tmp;
	if (!host_json_parse_string_in(obj, len, "tim_bmp", hex, sizeof(hex)) && hex[0]) {
		if (parse_hex(hex, v->tim_bmp, MAX_TIM_BMP, &decoded))
			return -1;
		if (decoded && !host_json_find_key_in(obj, len, "tim_bmp_len"))
			v->tim_bmp_len = (u8)decoded;
	}
	host_json_parse_int_in(obj, len, "expect_len", &v->expect_len);
	host_json_parse_string_in(obj, len, "expect_ie", v->expect_ie, sizeof(v->expect_ie));
	if (!host_json_parse_int_in(obj, len, "vap_map", &tmp))
		v->vap_map = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "vap_id", &tmp))
		v->vap_id = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_vap_id", &tmp))
		v->expect_vap_id = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_vap_map", &tmp))
		v->expect_vap_map = (u8)tmp;
	host_json_parse_int_in(obj, len, "expect_ret", &v->expect_ret);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct dvobj_priv dvobj;
	u8 tim_ie[MAX_IE], got;
	char got_hex[64];
	size_t i;

	if (!strcmp(v->fn, "rtw_set_tim_ie")) {
		got = rtw_set_tim_ie(v->dtim_cnt, v->dtim_period, v->tim_bmp,
				     v->tim_bmp_len, tim_ie);
		if ((int)got != v->expect_len) {
			fprintf(stderr, "%s: len got %u expect %d\n", v->name, got,
				v->expect_len);
			return -1;
		}
		for (i = 0; i < got && i * 2 + 2 <= sizeof(got_hex); i++)
			sprintf(got_hex + i * 2, "%02x", tim_ie[i]);
		got_hex[i * 2] = '\0';
		if (strcmp(got_hex, v->expect_ie)) {
			fprintf(stderr, "%s: ie got %s expect %s\n", v->name,
				got_hex, v->expect_ie);
			return -1;
		}
		return 0;
	}
	host_ap_rest_set_vap_map(&dvobj, v->vap_map);
	if (!strcmp(v->fn, "rtw_ap_allocate_vapid")) {
		got = rtw_ap_allocate_vapid(&dvobj);
		if (got != v->expect_vap_id ||
		    host_ap_rest_get_vap_map(&dvobj) != v->expect_vap_map) {
			fprintf(stderr, "%s: allocate mismatch\n", v->name);
			return -1;
		}
		return 0;
	}
	got = rtw_ap_release_vapid(&dvobj, v->vap_id);
	if ((int)got != v->expect_ret ||
	    host_ap_rest_get_vap_map(&dvobj) != v->expect_vap_map) {
		fprintf(stderr, "%s: release mismatch\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "ap_rest_vectors.json";

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
