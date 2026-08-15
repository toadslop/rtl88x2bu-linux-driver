// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for VHT MCS/rate pure helpers (W3-45). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long long u64;

#define MAX_VECTORS 32
#define MAX_NAME 128

enum vht_mcs_fn {
	FN_GET_HIGHEST_RATE = 0,
	FN_MCSMAP_TO_NSS,
	FN_MCS_TO_DATA_RATE,
	FN_MCS_MAP_TO_BITMAP,
};

struct vector {
	char name[MAX_NAME];
	enum vht_mcs_fn fn;
	u8 mcs_map[2];
	u8 bw;
	u8 short_gi;
	u8 vht_mcs_rate;
	u8 nss;
	int expect;
	unsigned expect_u16;
	unsigned long long expect_u64;
};

u8 rtw_get_vht_highest_rate(u8 *pvht_mcs_map);
u8 rtw_vht_mcsmap_to_nss(u8 *pvht_mcs_map);
u16 rtw_vht_mcs_to_data_rate(u8 bw, u8 short_GI, u8 vht_mcs_rate);
u64 rtw_vht_mcs_map_to_bitmap(u8 *mcs_map, u8 nss);

static int parse_fn(const char *obj, size_t len, enum vht_mcs_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_get_vht_highest_rate"))
		*out = FN_GET_HIGHEST_RATE;
	else if (!strcmp(fn, "rtw_vht_mcsmap_to_nss"))
		*out = FN_MCSMAP_TO_NSS;
	else if (!strcmp(fn, "rtw_vht_mcs_to_data_rate"))
		*out = FN_MCS_TO_DATA_RATE;
	else if (!strcmp(fn, "rtw_vht_mcs_map_to_bitmap"))
		*out = FN_MCS_MAP_TO_BITMAP;
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

static int parse_u64_field(const char *obj, size_t len, const char *key,
			   unsigned long long *out)
{
	char s[32];

	if (host_json_parse_string_in(obj, len, key, s, sizeof(s)))
		return -1;
	*out = strtoull(s, NULL, 0);
	return 0;
}

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, len, &v->fn))
		return -1;

	if (v->fn == FN_MCS_TO_DATA_RATE) {
		if (host_json_parse_int_in(obj, len, "bw", &tmp))
			return -1;
		v->bw = (u8)tmp;
		if (host_json_parse_int_in(obj, len, "short_gi", &tmp))
			return -1;
		v->short_gi = (u8)tmp;
		if (host_json_parse_int_in(obj, len, "vht_mcs_rate", &tmp))
			return -1;
		v->vht_mcs_rate = (u8)tmp;
		return host_json_parse_int_in(obj, len, "expect", (int *)&v->expect_u16);
	}

	if (parse_mcs_map_hex(obj, len, "mcs_map_hex", v->mcs_map))
		return -1;

	if (v->fn == FN_MCS_MAP_TO_BITMAP) {
		if (host_json_parse_int_in(obj, len, "nss", &tmp))
			return -1;
		v->nss = (u8)tmp;
		return parse_u64_field(obj, len, "expect_bitmap", &v->expect_u64);
	}

	return host_json_parse_int_in(obj, len, "expect", &v->expect);
}

static int run_vector(const struct vector *v)
{
	switch (v->fn) {
	case FN_GET_HIGHEST_RATE: {
		u8 got = rtw_get_vht_highest_rate((u8 *)v->mcs_map);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: highest rate got 0x%02x expected 0x%02x\n",
				v->name, got, v->expect);
			return -1;
		}
		return 0;
	}
	case FN_MCSMAP_TO_NSS: {
		u8 got = rtw_vht_mcsmap_to_nss((u8 *)v->mcs_map);

		if ((int)got != v->expect) {
			fprintf(stderr, "%s: nss got %u expected %d\n", v->name, got,
				v->expect);
			return -1;
		}
		return 0;
	}
	case FN_MCS_TO_DATA_RATE: {
		u16 got = rtw_vht_mcs_to_data_rate(v->bw, v->short_gi, v->vht_mcs_rate);

		if (got != v->expect_u16) {
			fprintf(stderr, "%s: rate got %u expected %u\n", v->name, got,
				v->expect_u16);
			return -1;
		}
		return 0;
	}
	case FN_MCS_MAP_TO_BITMAP: {
		u64 got = rtw_vht_mcs_map_to_bitmap((u8 *)v->mcs_map, v->nss);

		if (got != v->expect_u64) {
			fprintf(stderr, "%s: bitmap got 0x%llx expected 0x%llx\n", v->name,
				(unsigned long long)got,
				(unsigned long long)v->expect_u64);
			return -1;
		}
		return 0;
	}
	default:
		return -1;
	}
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
