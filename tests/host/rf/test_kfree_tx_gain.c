// SPDX-License-Identifier: GPL-2.0
/* Host L2 oracle runner for W3-59 rtw_rf_get_kfree_tx_gain_offset (PR1). */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_rf_kfree_tx_gain_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 16
#define MAX_NAME 128

struct vector {
	char name[MAX_NAME];
	u8 path, ch, kfree_flag;
	s8 bb_gain_val;
	int expect;
};

static int parse_vector_object(const char *obj, size_t len, void *vec_void)
{
	struct vector *v = vec_void;
	int tmp = 0;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (!host_json_parse_int_in(obj, len, "path", &tmp))
		v->path = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "ch", &tmp))
		v->ch = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "kfree_flag", &tmp))
		v->kfree_flag = (u8)tmp;
	if (!host_json_parse_int_in(obj, len, "bb_gain_val", &tmp))
		v->bb_gain_val = (s8)tmp;
	host_json_parse_int_in(obj, len, "expect", &v->expect);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct _adapter adapter;
	struct kfree_data_t *kfree_data;

	memset(&adapter, 0, sizeof(adapter));
	kfree_data = GET_KFREE_DATA(&adapter);
	if (v->kfree_flag) {
		int bb_gain_sel = rtw_ch_to_bb_gain_sel(v->ch);

		kfree_data->flag = KFREE_FLAG_ON;
		if (bb_gain_sel >= 0 && bb_gain_sel < BB_GAIN_NUM && v->path < RF_PATH_MAX)
			kfree_data->bb_gain[bb_gain_sel][v->path] = v->bb_gain_val;
	}

	if (rtw_rf_get_kfree_tx_gain_offset(&adapter, v->path, v->ch) != (s8)v->expect) {
		fprintf(stderr, "FAIL %s\n", v->name);
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	size_t nvec = 0;
	int failed = 0;
	const char *path = (argc > 1) ? argv[1] : "kfree_tx_gain_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec))
		return 2;
	for (size_t i = 0; i < nvec; i++)
		failed += run_vector(&vectors[i]) != 0;
	if (failed)
		return 1;
	printf("PASS %zu vectors (%s)\n", nvec, path);
	return 0;
}
