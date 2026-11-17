// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_iqk_trigger_dbg_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_iqk_trigger_dbg_trace
	host_hal_dm_phydm_iqk_trigger_dbg_trace;

void host_hal_dm_phydm_iqk_trigger_dbg_reset(void)
{
	memset(&host_hal_dm_phydm_iqk_trigger_dbg_trace, 0,
	       sizeof(host_hal_dm_phydm_iqk_trigger_dbg_trace));
}

void halrf_segment_iqk_trigger(void *dm_void, boolean clear, boolean segment)
{
	(void)dm_void;
	host_hal_dm_phydm_iqk_trigger_dbg_trace.segment_clear = clear;
	host_hal_dm_phydm_iqk_trigger_dbg_trace.segment_flag = segment;
	host_hal_dm_phydm_iqk_trigger_dbg_trace.segment_calls++;
}

void halrf_iqk_trigger(void *dm_void, boolean is_recovery)
{
	(void)dm_void;
	host_hal_dm_phydm_iqk_trigger_dbg_trace.plain_iqk_recovery =
		is_recovery;
	host_hal_dm_phydm_iqk_trigger_dbg_trace.plain_iqk_calls++;
}

struct vector {
	char name[48];
	int recovery;
	int clear;
	int segment;
	int expect_segment;
	int expect_segment_clear;
	int expect_segment_flag;
	int expect_plain_iqk;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"recovery", "clear", "segment", "expect_segment",
		"expect_segment_clear", "expect_segment_flag", "expect_plain_iqk",
	};
	int *dst[] = {
		&v->recovery, &v->clear, &v->segment, &v->expect_segment,
		&v->expect_segment_clear, &v->expect_segment_flag,
		&v->expect_plain_iqk,
	};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
		if (host_json_parse_int_in(obj, len, keys[i], dst[i]))
			return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_iqk_trigger_dbg_trace *t =
		&host_hal_dm_phydm_iqk_trigger_dbg_trace;

	host_hal_dm_phydm_iqk_trigger_dbg_reset();
	memset(&adapter, 0, sizeof(adapter));

	rtw_phydm_iqk_trigger_dbg(&adapter, (boolean)v->recovery,
				  (boolean)v->clear, (boolean)v->segment);

	if (t->segment_calls != (unsigned)v->expect_segment)
		return -1;
	if (t->plain_iqk_calls != (unsigned)v->expect_plain_iqk)
		return -1;
	if (v->expect_segment && t->segment_clear != (u8)v->expect_segment_clear)
		return -1;
	if (v->expect_segment && t->segment_flag != (u8)v->expect_segment_flag)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_phydm_iqk_trigger_dbg_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}