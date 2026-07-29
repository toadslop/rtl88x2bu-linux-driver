// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_ieee80211_rest cipher suite getters (W3-27).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_ieee80211_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 64
#define MAX_NAME 128

enum ie_rest_cipher_fn {
	FN_WPA_CIPHER = 0,
	FN_RSN_CIPHER,
	FN_AKM_BITMAP,
};

struct vector {
	char name[MAX_NAME];
	enum ie_rest_cipher_fn fn;
	u8 suite[4];
	u32 expect;
};

static int parse_fn(const char *obj, size_t obj_len, enum ie_rest_cipher_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_get_wpa_cipher_suite") == 0)
		*out = FN_WPA_CIPHER;
	else if (strcmp(fn, "rtw_get_rsn_cipher_suite") == 0)
		*out = FN_RSN_CIPHER;
	else if (strcmp(fn, "rtw_get_akm_suite_bitmap") == 0)
		*out = FN_AKM_BITMAP;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t suite_len = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "expect", (int *)&v->expect);
	if (host_json_parse_string_in(obj, obj_len, "suite", hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, v->suite, sizeof(v->suite), &suite_len) ||
	    suite_len != 4)
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	switch (v->fn) {
	case FN_WPA_CIPHER: {
		int ret = rtw_get_wpa_cipher_suite(v->suite);

		if ((u32)ret != v->expect) {
			fprintf(stderr, "%s: wpa_cipher got %d expect %u\n", v->name,
				ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_RSN_CIPHER: {
		int ret = rtw_get_rsn_cipher_suite(v->suite);

		if ((u32)ret != v->expect) {
			fprintf(stderr, "%s: rsn_cipher got %d expect %u\n", v->name,
				ret, v->expect);
			return -1;
		}
		break;
	}
	case FN_AKM_BITMAP: {
		u32 ret = rtw_get_akm_suite_bitmap(v->suite);

		if (ret != v->expect) {
			fprintf(stderr, "%s: akm_bitmap got %u expect %u\n", v->name,
				ret, v->expect);
			return -1;
		}
		break;
	}
	default:
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "ie_rest_cipher_vectors.json";
	struct vector vectors[MAX_VECTORS];
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
	printf("all %zu ieee80211_rest cipher vectors passed (oracle: core/rtw_ieee80211_rest.c)\n",
	       nvec);
	return 0;
}
