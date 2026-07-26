// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_chplan.c lookup helpers (W2-17a).
 *
 * oracle: core/rtw_chplan.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_chplan_types.h"
#include "host_vector_json.h"
#include "rtw_chplan.h"

u8 rtw_chplan_get_default_regd_2g(u8 id);
u8 rtw_chplan_get_default_regd_5g(u8 id);

#define MAX_VECTORS 64
#define MAX_NAME 128

enum chplan_fn {
	FN_REGD_2G = 0,
	FN_REGD_5G,
	FN_REGD,
	FN_IS_EMPTY,
	FN_IS_VALID,
	FN_EXCL_CHS,
};

struct vector {
	char name[MAX_NAME];
	enum chplan_fn fn;
	char obj[4096];
	size_t obj_len;
	int id;
	int ch;
	int expect;
	u8 excl_chs[MAX_CHANNEL_NUM];
};

static int parse_fn(const char *obj, size_t obj_len, enum chplan_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_chplan_get_default_regd_2g") == 0)
		*out = FN_REGD_2G;
	else if (strcmp(fn, "rtw_chplan_get_default_regd_5g") == 0)
		*out = FN_REGD_5G;
	else if (strcmp(fn, "rtw_chplan_get_default_regd") == 0)
		*out = FN_REGD;
	else if (strcmp(fn, "rtw_chplan_is_empty") == 0)
		*out = FN_IS_EMPTY;
	else if (strcmp(fn, "rtw_is_channel_plan_valid") == 0)
		*out = FN_IS_VALID;
	else if (strcmp(fn, "rtw_regsty_is_excl_chs") == 0)
		*out = FN_EXCL_CHS;
	else
		return -1;
	return 0;
}

static int parse_excl_chs(const char *hex, u8 *out)
{
	size_t n = 0;
	size_t len = 0;
	char byte[3];
	size_t i;

	if (!hex || !*hex)
		return 0;

	memset(out, 0, MAX_CHANNEL_NUM);
	for (i = 0; hex[i]; i++) {
		if (hex[i] == ' ' || hex[i] == '\t')
			continue;
		byte[n++] = hex[i];
		if (n == 2) {
			unsigned int val;

			byte[2] = '\0';
			if (sscanf(byte, "%x", &val) != 1)
				return -1;
			if (len >= MAX_CHANNEL_NUM)
				return -1;
			out[len++] = (u8)val;
			n = 0;
		}
	}
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;
	char hex[HOST_VECTOR_MAX_HEX_BUF];

	memset(v, 0, sizeof(*v));
	if (obj_len >= sizeof(v->obj))
		return -1;
	memcpy(v->obj, obj, obj_len);
	v->obj_len = obj_len;
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "id", &v->id);
	host_json_parse_int_in(obj, obj_len, "ch", &v->ch);
	host_json_parse_int_in(obj, obj_len, "expect", &v->expect);
	if (!host_json_parse_string_in(obj, obj_len, "excl_chs", hex, sizeof(hex))) {
		if (parse_excl_chs(hex, v->excl_chs))
			return -1;
	}
	return 0;
}

static int run_vector(const struct vector *v)
{
	struct registry_priv regsty;

	memset(&regsty, 0, sizeof(regsty));
	_rtw_memcpy(regsty.excl_chs, v->excl_chs, sizeof(regsty.excl_chs));

	switch (v->fn) {
	case FN_REGD_2G:
		if ((int)rtw_chplan_get_default_regd_2g((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd_2g mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_REGD_5G:
		if ((int)rtw_chplan_get_default_regd_5g((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd_5g mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_REGD:
		if ((int)rtw_chplan_get_default_regd((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: regd mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_IS_EMPTY:
		if ((int)rtw_chplan_is_empty((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: is_empty mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_IS_VALID:
		if ((int)rtw_is_channel_plan_valid((u8)v->id) != v->expect) {
			fprintf(stderr, "%s: is_valid mismatch\n", v->name);
			return -1;
		}
		break;
	case FN_EXCL_CHS:
		if ((int)rtw_regsty_is_excl_chs(&regsty, (u8)v->ch) != v->expect) {
			fprintf(stderr, "%s: excl_chs mismatch\n", v->name);
			return -1;
		}
		break;
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const char *path = "chplan_vectors.json";
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
	printf("all %zu chplan vectors passed (oracle: core/rtw_chplan.c)\n", nvec);
	return 0;
}
