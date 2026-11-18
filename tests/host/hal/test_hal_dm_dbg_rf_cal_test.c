// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_dbg_rf_cal_test_types.h"
#include "host_vector_json.h"

struct host_hal_dm_dbg_rf_cal_trace host_hal_dm_dbg_rf_cal_trace;

void host_hal_dm_dbg_rf_cal_reset(void)
{
	memset(&host_hal_dm_dbg_rf_cal_trace, 0, sizeof(host_hal_dm_dbg_rf_cal_trace));
}

void host_hal_dm_dbg_rf_cal_record_step(u8 step)
{
	struct host_hal_dm_dbg_rf_cal_trace *t = &host_hal_dm_dbg_rf_cal_trace;

	if (t->step_count < HOST_DBG_RF_MAX_STEPS)
		t->steps[t->step_count++] = step;
}

struct vector {
	char name[48];
	char op[16];
	int recovery;
	int clear;
	int segment;
	int expect_step_count;
	int expect_steps[HOST_DBG_RF_MAX_STEPS];
	u64 expect_ability_mask;
	int expect_iqk_dbg;
	int expect_lck_trigger;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	char step_key[24];
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	if (host_json_parse_int_in(obj, len, "recovery", &v->recovery))
		v->recovery = 0;
	if (host_json_parse_int_in(obj, len, "clear", &v->clear))
		v->clear = 0;
	if (host_json_parse_int_in(obj, len, "segment", &v->segment))
		v->segment = 0;
	if (host_json_parse_int_in(obj, len, "expect_step_count",
				  &v->expect_step_count))
		return -1;
	for (i = 0; i < HOST_DBG_RF_MAX_STEPS; i++) {
		snprintf(step_key, sizeof(step_key), "expect_step_%zu", i);
		if (host_json_parse_int_in(obj, len, step_key, &v->expect_steps[i]))
			v->expect_steps[i] = -1;
	}
	{
		unsigned long mask_ul;

		if (host_json_parse_ulong_in(obj, len, "expect_ability_mask",
					   &mask_ul))
			return -1;
		v->expect_ability_mask = mask_ul;
	}
	if (host_json_parse_int_in(obj, len, "expect_iqk_dbg", &v->expect_iqk_dbg))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_lck_trigger",
				  &v->expect_lck_trigger))
		return -1;
	return 0;
}

static int steps_match(struct vector *v)
{
	struct host_hal_dm_dbg_rf_cal_trace *t = &host_hal_dm_dbg_rf_cal_trace;
	int i;

	if (t->step_count != (u8)v->expect_step_count)
		return 0;
	for (i = 0; i < v->expect_step_count; i++)
		if (t->steps[i] != (u8)v->expect_steps[i])
			return 0;
	return 1;
}

static int run_vector(struct vector *v)
{
	host_hal_dm_dbg_rf_cal_adapter adapter;
	struct host_hal_dm_dbg_rf_cal_trace *t;

	host_hal_dm_dbg_rf_cal_reset();
	memset(&adapter, 0, sizeof(adapter));

	if (!strcmp(v->op, "iqk_test"))
		rtw_hal_iqk_test(&adapter, v->recovery != 0, v->clear != 0,
				 v->segment != 0);
	else if (!strcmp(v->op, "lck_test"))
		rtw_hal_lck_test(&adapter);
	else
		return fprintf(stderr, "unknown op %s\n", v->op), 1;

	t = &host_hal_dm_dbg_rf_cal_trace;
	if (!steps_match(v) ||
	    t->halrf_ability_mask != v->expect_ability_mask ||
	    t->iqk_dbg_calls != (u8)v->expect_iqk_dbg ||
	    t->lck_trigger_calls != (u8)v->expect_lck_trigger) {
		fprintf(stderr,
			"FAIL %s steps=%u ability=%llu iqk=%u lck=%u\n",
			v->name, t->step_count,
			(unsigned long long)t->halrf_ability_mask,
			t->iqk_dbg_calls, t->lck_trigger_calls);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[4];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_dbg_rf_cal_test_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 4,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}