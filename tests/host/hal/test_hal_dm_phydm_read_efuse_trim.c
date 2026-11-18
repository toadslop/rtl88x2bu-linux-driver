// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_read_efuse_trim_types.h"
#include "host_vector_json.h"

#ifndef RUST_HAL_DM_PHYDM_READ_EFUSE_TRIM_ORACLE
struct host_hal_dm_phydm_read_efuse_trim_trace
	host_hal_dm_phydm_read_efuse_trim_trace;

void host_hal_dm_phydm_read_efuse_trim_reset(void)
{
	memset(&host_hal_dm_phydm_read_efuse_trim_trace, 0,
	       sizeof(host_hal_dm_phydm_read_efuse_trim_trace));
}

void phydm_get_thermal_trim_offset(void *dm_void)
{
	(void)dm_void;
	host_hal_dm_phydm_read_efuse_trim_trace.thermal_trim_calls++;
}

void phydm_get_power_trim_offset(void *dm_void)
{
	(void)dm_void;
	host_hal_dm_phydm_read_efuse_trim_trace.power_trim_calls++;
}
#endif

struct vector {
	char name[48];
	int expect_thermal;
	int expect_power;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_thermal", &v->expect_thermal))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_power", &v->expect_power))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_read_efuse_trim_trace *t =
		&host_hal_dm_phydm_read_efuse_trim_trace;

	host_hal_dm_phydm_read_efuse_trim_reset();
	memset(&adapter, 0, sizeof(adapter));

	rtw_phydm_read_efuse(&adapter);

	if (t->thermal_trim_calls != (unsigned)v->expect_thermal)
		return -1;
	if (t->power_trim_calls != (unsigned)v->expect_power)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[4];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_phydm_read_efuse_trim_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 4,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}