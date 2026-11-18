// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_com_update_sta_ra_info_types.h"
#include "host_vector_json.h"

struct host_hal_com_update_sta_ra_info_trace host_hal_com_update_sta_ra_info_trace;

void host_hal_com_update_sta_ra_info_reset(void)
{
	memset(&host_hal_com_update_sta_ra_info_trace, 0,
	       sizeof(host_hal_com_update_sta_ra_info_trace));
}

void host_hal_com_update_sta_ra_info_record_step(u8 step)
{
	struct host_hal_com_update_sta_ra_info_trace *t = &host_hal_com_update_sta_ra_info_trace;

	if (t->step_count < HOST_RA_MAX_STEPS)
		t->steps[t->step_count++] = step;
}

struct vector {
	char name[48];
	int expect_steps[HOST_RA_MAX_STEPS];
	int expect_step_count;
	int expect_mimo;
	int expect_smps;
	int expect_rate_mask;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"expect_step_count", "expect_step_0", "expect_step_1", "expect_step_2",
		"expect_mimo", "expect_smps", "expect_rate_mask",
	};
	int *dst[] = {
		&v->expect_step_count, &v->expect_steps[0], &v->expect_steps[1],
		&v->expect_steps[2], &v->expect_mimo, &v->expect_smps,
		&v->expect_rate_mask,
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

static int steps_match(struct vector *v)
{
	struct host_hal_com_update_sta_ra_info_trace *t = &host_hal_com_update_sta_ra_info_trace;
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
	struct host_adapter_ra adapter;
	struct host_sta_ra sta;
	struct host_hal_com_update_sta_ra_info_trace *t;

	host_hal_com_update_sta_ra_info_reset();
	memset(&adapter, 0, sizeof(adapter));
	memset(&sta, 0, sizeof(sta));

	rtw_hal_update_sta_ra_info(&adapter, &sta);
	t = &host_hal_com_update_sta_ra_info_trace;

	if (!steps_match(v) ||
	    t->mimo_calls != (u8)v->expect_mimo ||
	    t->smps_calls != (u8)v->expect_smps ||
	    t->rate_mask_calls != (u8)v->expect_rate_mask) {
		fprintf(stderr,
			"FAIL %s steps=%u mimo=%u smps=%u rate=%u\n",
			v->name, t->step_count, t->mimo_calls, t->smps_calls,
			t->rate_mask_calls);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[4];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] :
			   "hal_com_update_sta_ra_info_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 4,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}