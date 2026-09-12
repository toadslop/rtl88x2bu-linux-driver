// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_ap_bmc_rate_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 8
#define MAX_NAME 64

struct vector {
	char name[MAX_NAME];
	char fn[32];
	u8 band, tx_rate, expect_rate;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)) ||
	    host_json_parse_string_in(obj, len, "fn", v->fn, sizeof(v->fn)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "band", &tmp))
		v->band = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "tx_rate", &tmp))
		v->tx_rate = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "expect_rate", &tmp))
		v->expect_rate = (u8)tmp;
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct _adapter adapter;
	u8 got;

	memset(&adapter, 0, sizeof(adapter));
	adapter.hal_data.current_band_type = v->band;
	if (strcmp(v->fn, "rtw_ap_find_bmc_rate"))
		return -1;
	got = rtw_ap_find_bmc_rate(&adapter, v->tx_rate);
	if (got != v->expect_rate) {
		fprintf(stderr, "FAIL %s: expect %u got %u\n", v->name, v->expect_rate, got);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t n = 0, i, fail = 0;

	if (argc != 2)
		return 2;
	if (host_load_vectors(argv[1], vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &n))
		return 2;
	for (i = 0; i < n; i++)
		if (run_vector(&vectors[i]))
			fail++;
	printf("%zu vectors, %zu failures\n", n, fail);
	return fail ? 1 : 0;
}
