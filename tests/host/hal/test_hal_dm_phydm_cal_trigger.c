// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_cal_trigger_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_cal_trigger_trace host_hal_dm_phydm_cal_trigger_trace;

void host_hal_dm_phydm_cal_trigger_reset(void)
{
	memset(&host_hal_dm_phydm_cal_trigger_trace, 0,
	       sizeof(host_hal_dm_phydm_cal_trigger_trace));
}

void host_hal_dm_phydm_cal_trigger_record_step(u8 step)
{
	struct host_hal_dm_phydm_cal_trigger_trace *t = &host_hal_dm_phydm_cal_trigger_trace;

	if (t->step_count < HOST_CAL_MAX_STEPS)
		t->steps[t->step_count++] = step;
}

struct vector {
	char name[48];
	int expect_steps[HOST_CAL_MAX_STEPS];
	int expect_step_count;
	int expect_ps_deny_ioctl_set;
	int expect_leave_ps;
	int expect_iqk_all;
	int expect_ps_deny_clear;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {
		"expect_step_count", "expect_step_0", "expect_step_1",
		"expect_step_2", "expect_step_3", "expect_ps_deny_ioctl_set",
		"expect_leave_ps", "expect_iqk_all", "expect_ps_deny_clear",
	};
	int *dst[] = {
		&v->expect_step_count, &v->expect_steps[0], &v->expect_steps[1],
		&v->expect_steps[2], &v->expect_steps[3], &v->expect_ps_deny_ioctl_set,
		&v->expect_leave_ps, &v->expect_iqk_all, &v->expect_ps_deny_clear,
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
	struct host_hal_dm_phydm_cal_trigger_trace *t = &host_hal_dm_phydm_cal_trigger_trace;
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
	host_hal_dm_cal_adapter adapter;
	struct host_hal_dm_phydm_cal_trigger_trace *t;
	u32 deny;

	host_hal_dm_phydm_cal_trigger_reset();
	memset(&adapter, 0, sizeof(adapter));

	rtw_hal_phydm_cal_trigger(&adapter);
	t = &host_hal_dm_phydm_cal_trigger_trace;
	deny = rtw_ps_deny_get(&adapter);

	if (!steps_match(v) ||
	    t->leave_ps_direct_calls != (u8)v->expect_leave_ps ||
	    t->iqk_all_calls != (u8)v->expect_iqk_all ||
	    (v->expect_ps_deny_ioctl_set &&
	     t->ps_deny_reason != HOST_PS_DENY_IOCTL) ||
	    (v->expect_ps_deny_clear && deny != 0)) {
		fprintf(stderr,
			"FAIL %s steps=%u leave=%u iqk=%u deny=0x%x\n",
			v->name, t->step_count, t->leave_ps_direct_calls,
			t->iqk_all_calls, deny);
		return 1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[4];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_phydm_cal_trigger_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 4,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}