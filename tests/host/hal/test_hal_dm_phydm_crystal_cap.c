// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_crystal_cap_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_crystal_cap_trace host_hal_dm_phydm_crystal_cap_trace;

void host_hal_dm_phydm_crystal_cap_reset(void)
{
	memset(&host_hal_dm_phydm_crystal_cap_trace, 0,
	       sizeof(host_hal_dm_phydm_crystal_cap_trace));
}

boolean phydm_set_crystal_cap_reg(void *dm_void, u8 crystal_cap)
{
	(void)dm_void;
	host_hal_dm_phydm_crystal_cap_trace.last_cap = crystal_cap;
	host_hal_dm_phydm_crystal_cap_trace.set_reg_calls++;
	return host_hal_dm_phydm_crystal_cap_trace.stub_return;
}

struct vector {
	char name[48];
	int crystal_cap;
	int stub_ok;
	int expect_cap;
	int expect_result;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "crystal_cap", &ul))
		return -1;
	v->crystal_cap = (int)ul;
	if (host_json_parse_int_in(obj, len, "stub_ok", &v->stub_ok))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_cap", &ul))
		return -1;
	v->expect_cap = (int)ul;
	if (host_json_parse_int_in(obj, len, "expect_result", &v->expect_result))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	boolean ret;
	struct host_hal_dm_phydm_crystal_cap_trace *t =
		&host_hal_dm_phydm_crystal_cap_trace;

	host_hal_dm_phydm_crystal_cap_reset();
	memset(&adapter, 0, sizeof(adapter));
	t->stub_return = (u8)v->stub_ok;

	ret = rtw_phydm_set_crystal_cap(&adapter, (u8)v->crystal_cap);
	if (ret != (boolean)v->expect_result)
		return -1;
	if (t->set_reg_calls != 1)
		return -1;
	if (t->last_cap != (u8)v->expect_cap)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_dm_phydm_crystal_cap_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}