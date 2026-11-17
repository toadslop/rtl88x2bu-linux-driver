// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_rrsr_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_rrsr_trace host_hal_dm_phydm_rrsr_trace;

void host_hal_dm_phydm_rrsr_reset(void)
{
	memset(&host_hal_dm_phydm_rrsr_trace, 0,
	       sizeof(host_hal_dm_phydm_rrsr_trace));
}

void odm_cmn_info_update(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	host_hal_dm_phydm_rrsr_trace.cmn_update_info = cmn_info;
	host_hal_dm_phydm_rrsr_trace.cmn_update_value = value;
	host_hal_dm_phydm_rrsr_trace.cmn_update_calls++;
}

void phydm_rrsr_set_register(void *dm_void, u32 rrsr_val)
{
	(void)dm_void;
	host_hal_dm_phydm_rrsr_trace.reg_set_value = rrsr_val;
	host_hal_dm_phydm_rrsr_trace.reg_set_calls++;
}

void phydm_rrsr_en(void *dm_void, boolean en_rrsr)
{
	(void)dm_void;
	host_hal_dm_phydm_rrsr_trace.rrsr_en = en_rrsr;
	host_hal_dm_phydm_rrsr_trace.rrsr_en_calls++;
}

struct vector {
	char name[48];
	char op[16];
	u32 reg_set_rrsr;
	u32 rrsr_arg;
	int write_rrsr;
	int en_rrsr;
	u32 expect_rrsr;
	int expect_cmn_update;
	int expect_reg_set;
	int expect_rrsr_en;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_string_in(obj, len, "op", v->op, sizeof(v->op)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "reg_set_rrsr", &ul))
		return -1;
	v->reg_set_rrsr = (u32)ul;
	if (host_json_parse_ulong_in(obj, len, "rrsr_arg", &ul))
		return -1;
	v->rrsr_arg = (u32)ul;
	if (host_json_parse_ulong_in(obj, len, "expect_rrsr", &ul))
		return -1;
	v->expect_rrsr = (u32)ul;
	if (host_json_parse_int_in(obj, len, "write_rrsr", &v->write_rrsr))
		return -1;
	if (host_json_parse_int_in(obj, len, "en_rrsr", &v->en_rrsr))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_cmn_update", &v->expect_cmn_update))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_reg_set", &v->expect_reg_set))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_rrsr_en", &v->expect_rrsr_en))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_rrsr_trace *t = &host_hal_dm_phydm_rrsr_trace;

	host_hal_dm_phydm_rrsr_reset();
	memset(&adapter, 0, sizeof(adapter));
	adapter.registrypriv.set_rrsr_value = v->reg_set_rrsr;

	if (!strcmp(v->op, "set_rrsr")) {
		rtw_phydm_set_rrsr(&adapter, v->rrsr_arg, (boolean)v->write_rrsr);
		if (t->cmn_update_calls != (u8)v->expect_cmn_update)
			return -1;
		if (v->expect_cmn_update &&
		    t->cmn_update_info != ODM_CMNINFO_RRSR_VAL)
			return -1;
		if (v->expect_cmn_update &&
		    t->cmn_update_value != (u64)v->expect_rrsr)
			return -1;
		if (t->reg_set_calls != (u8)v->expect_reg_set)
			return -1;
		if (v->expect_reg_set && t->reg_set_value != v->expect_rrsr)
			return -1;
	} else if (!strcmp(v->op, "dyn_en")) {
		rtw_phydm_dyn_rrsr_en(&adapter, (boolean)v->en_rrsr);
		if (t->rrsr_en_calls != 1)
			return -1;
		if (t->rrsr_en != (u8)v->expect_rrsr_en)
			return -1;
	} else {
		return -1;
	}
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_phydm_rrsr_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}