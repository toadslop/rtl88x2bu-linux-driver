// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for VHT MCS map helpers (W3-45 PR1). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

typedef unsigned char u8;

#define MAX_VECTORS 32
#define MAX_NAME 128

enum vht_mcs_fn {
	FN_GET_HIGHEST_RATE = 0,
	FN_MCSMAP_TO_NSS,
};

struct vector {
	char name[MAX_NAME];
	enum vht_mcs_fn fn;
	u8 mcs_map[2];
	int expect;
};

u8 rtw_get_vht_highest_rate(u8 *pvht_mcs_map);
u8 rtw_vht_mcsmap_to_nss(u8 *pvht_mcs_map);

static int parse_fn(const char *obj, size_t len, enum vht_mcs_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_get_vht_highest_rate"))
		*out = FN_GET_HIGHEST_RATE;
	else if (!strcmp(fn, "rtw_vht_mcsmap_to_nss"))
		*out = FN_MCSMAP_TO_NSS;
	else
		return -1;
	return 0;
}

static int parse_mcs_map_hex(const char *obj, size_t len, const char *key, u8 *out)
{
	char hex[HOST_VECTOR_MAX_HEX_BUF];
	size_t decoded = 0;

	if (host_json_parse_string_in(obj, len, key, hex, sizeof(hex)))
		return -1;
	if (host_hex_decode(hex, out, 2, &decoded) || decoded != 2)
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;
	if (parse_mcs_map_hex(obj, len, "mcs_map_hex", v->mcs_map))
		return -1;
	return host_json_parse_int_in(obj, len, "expect", &v->expect);
}

static int run_vector(const struct vector *v)
{
	u8 got;

	if (v->fn == FN_GET_HIGHEST_RATE)
		got = rtw_get_vht_highest_rate((u8 *)v->mcs_map);
	else
		got = rtw_vht_mcsmap_to_nss((u8 *)v->mcs_map);

	if ((int)got != v->expect) {
		fprintf(stderr, "%s: got %u expected %d\n", v->name, got, v->expect);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "vht_mcs_rate_vectors.json";

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
