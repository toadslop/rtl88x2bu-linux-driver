// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for VHT MCS/NSS helpers (W3-35). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_vector_json.h"

typedef unsigned char u8;

#define MAX_VECTORS 32
#define MAX_NAME 128

enum vht_fn { FN_NSS_TO_MCSMAP = 0, FN_GET_SS_FROM_MAP };

struct vector {
	char name[MAX_NAME];
	enum vht_fn fn;
	u8 nss;
	u8 cur_mcs_map[2];
	u8 expect_mcs_map[2];
	int expect;
};

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map);
u8 VHT_get_ss_from_map(u8 *vht_mcs_map);

static int parse_fn(const char *obj, size_t len, enum vht_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, len, "fn", fn, sizeof(fn)))
		return -1;
	if (!strcmp(fn, "rtw_vht_nss_to_mcsmap"))
		*out = FN_NSS_TO_MCSMAP;
	else if (!strcmp(fn, "VHT_get_ss_from_map"))
		*out = FN_GET_SS_FROM_MAP;
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
	if (parse_mcs_map_hex(obj, len, "cur_mcs_map_hex", v->cur_mcs_map))
		return -1;
	if (v->fn == FN_NSS_TO_MCSMAP) {
		int nss;
		if (host_json_parse_int_in(obj, len, "nss", &nss))
			return -1;
		v->nss = (u8)nss;
		return parse_mcs_map_hex(obj, len, "expect_mcs_map_hex", v->expect_mcs_map);
	}
	return host_json_parse_int_in(obj, len, "expect", &v->expect);
}

static int run_vector(const struct vector *v)
{
	if (v->fn == FN_NSS_TO_MCSMAP) {
		u8 target[2];
		rtw_vht_nss_to_mcsmap(v->nss, target, (u8 *)v->cur_mcs_map);
		if (target[0] != v->expect_mcs_map[0] || target[1] != v->expect_mcs_map[1]) {
			fprintf(stderr, "%s: got [%u,%u] expected [%u,%u]\n", v->name,
				target[0], target[1], v->expect_mcs_map[0], v->expect_mcs_map[1]);
			return -1;
		}
		return 0;
	}
	if ((int)VHT_get_ss_from_map((u8 *)v->cur_mcs_map) != v->expect) {
		fprintf(stderr, "%s: ss mismatch\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t count = 0, i;
	const char *path = (argc > 1) ? argv[1] : "vht_vectors.json";

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
