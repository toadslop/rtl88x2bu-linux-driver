// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 oracle runner for rtw_io_rest helpers (W3-18 PR1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_io_types.h"
#include "host_vector_json.h"

#define MAX_VECTORS 32
#define MAX_NAME 128

enum io_fn {
	FN_INC_CHK = 0,
	FN_RESET,
	FN_MATCH_READ,
	FN_MATCH_WRITE,
	FN_MATCH_RF_READ,
	FN_MATCH_RF_WRITE,
};

struct vector {
	char name[MAX_NAME];
	enum io_fn fn;
	int initial_count;
	int expect_ret;
	int expect_count;
	u8 chip;
	u8 hci;
	u32 addr;
	u16 len;
	u32 val;
	u8 path;
	u32 mask;
	u32 expect;
};

int rtw_inc_and_chk_continual_io_error(struct dvobj_priv *dvobj);
void rtw_reset_continual_io_error(struct dvobj_priv *dvobj);
u32 match_read_sniff(_adapter *adapter, u32 addr, u16 len, u32 val);
u32 match_write_sniff(_adapter *adapter, u32 addr, u16 len, u32 val);
bool match_rf_read_sniff_ranges(_adapter *adapter, u8 path, u32 addr, u32 mask);
bool match_rf_write_sniff_ranges(_adapter *adapter, u8 path, u32 addr, u32 mask);

static int parse_fn(const char *obj, size_t obj_len, enum io_fn *out)
{
	char fn[64];

	if (host_json_parse_string_in(obj, obj_len, "fn", fn, sizeof(fn)))
		return -1;
	if (strcmp(fn, "rtw_inc_and_chk_continual_io_error") == 0)
		*out = FN_INC_CHK;
	else if (strcmp(fn, "rtw_reset_continual_io_error") == 0)
		*out = FN_RESET;
	else if (strcmp(fn, "match_read_sniff") == 0)
		*out = FN_MATCH_READ;
	else if (strcmp(fn, "match_write_sniff") == 0)
		*out = FN_MATCH_WRITE;
	else if (strcmp(fn, "match_rf_read_sniff_ranges") == 0)
		*out = FN_MATCH_RF_READ;
	else if (strcmp(fn, "match_rf_write_sniff_ranges") == 0)
		*out = FN_MATCH_RF_WRITE;
	else
		return -1;
	return 0;
}

static int parse_vector_object(const char *obj, size_t obj_len, void *vec_void)
{
	struct vector *v = vec_void;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, obj_len, "name", v->name, sizeof(v->name)))
		return -1;
	if (parse_fn(obj, obj_len, &v->fn))
		return -1;
	host_json_parse_int_in(obj, obj_len, "initial_count", &v->initial_count);
	host_json_parse_int_in(obj, obj_len, "expect_ret", &v->expect_ret);
	host_json_parse_int_in(obj, obj_len, "expect_count", &v->expect_count);
	host_json_parse_int_in(obj, obj_len, "chip", (int *)&v->chip);
	host_json_parse_int_in(obj, obj_len, "hci", (int *)&v->hci);
	host_json_parse_int_in(obj, obj_len, "addr", (int *)&v->addr);
	host_json_parse_int_in(obj, obj_len, "len", (int *)&v->len);
	host_json_parse_int_in(obj, obj_len, "val", (int *)&v->val);
	host_json_parse_int_in(obj, obj_len, "path", (int *)&v->path);
	host_json_parse_int_in(obj, obj_len, "mask", (int *)&v->mask);
	host_json_parse_int_in(obj, obj_len, "expect", (int *)&v->expect);
	return 0;
}

static int run_vector(struct vector *v)
{
	struct dvobj_priv dvobj;
	_adapter adapter;

	memset(&dvobj, 0, sizeof(dvobj));
	memset(&adapter, 0, sizeof(adapter));
	adapter.dvobj = &dvobj;
	dvobj.continual_io_error = v->initial_count;
	dvobj.chip_type = v->chip;
	dvobj.interface_type = v->hci;

	switch (v->fn) {
	case FN_INC_CHK: {
		int ret = rtw_inc_and_chk_continual_io_error(&dvobj);

		if (ret != v->expect_ret || dvobj.continual_io_error != v->expect_count) {
			fprintf(stderr,
				"%s: inc_chk ret=%d count=%d expected ret=%d count=%d\n",
				v->name, ret, dvobj.continual_io_error, v->expect_ret,
				v->expect_count);
			return -1;
		}
		break;
	}
	case FN_RESET:
		rtw_reset_continual_io_error(&dvobj);
		if (dvobj.continual_io_error != v->expect_count) {
			fprintf(stderr, "%s: reset count=%d expected %d\n", v->name,
				dvobj.continual_io_error, v->expect_count);
			return -1;
		}
		break;
	case FN_MATCH_READ: {
		u32 got = match_read_sniff(&adapter, v->addr, v->len, v->val);

		if (got != v->expect) {
			fprintf(stderr, "%s: match_read_sniff got=%u expect=%u\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_MATCH_WRITE: {
		u32 got = match_write_sniff(&adapter, v->addr, v->len, v->val);

		if (got != v->expect) {
			fprintf(stderr, "%s: match_write_sniff got=%u expect=%u\n",
				v->name, got, v->expect);
			return -1;
		}
		break;
	}
	case FN_MATCH_RF_READ: {
		bool got = match_rf_read_sniff_ranges(&adapter, v->path, v->addr,
						      v->mask);

		if ((int)got != (int)v->expect) {
			fprintf(stderr, "%s: match_rf_read got=%d expect=%u\n",
				v->name, (int)got, v->expect);
			return -1;
		}
		break;
	}
	case FN_MATCH_RF_WRITE: {
		bool got = match_rf_write_sniff_ranges(&adapter, v->path, v->addr,
						       v->mask);

		if ((int)got != (int)v->expect) {
			fprintf(stderr, "%s: match_rf_write got=%d expect=%u\n",
				v->name, (int)got, v->expect);
			return -1;
		}
		break;
	}
	default:
		fprintf(stderr, "%s: unknown fn\n", v->name);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[MAX_VECTORS];
	const char *path = "io_vectors.json";
	size_t nvec = 0;
	size_t i;

	if (argc > 1)
		path = argv[1];
	if (host_load_vectors(path, vectors, sizeof(vectors[0]), MAX_VECTORS,
			      parse_vector_object, &nvec)) {
		fprintf(stderr, "failed to load %s\n", path);
		return 1;
	}
	for (i = 0; i < nvec; i++) {
		if (run_vector(&vectors[i])) {
			fprintf(stderr, "vector failed: %s\n", vectors[i].name);
			return 1;
		}
	}
	printf("ok: %zu vectors\n", nvec);
	return 0;
}
