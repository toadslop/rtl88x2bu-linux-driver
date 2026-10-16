// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_hal_dm_board_type_types.h"
#include "host_vector_json.h"

struct host_hal_dm_board_trace host_hal_dm_board_trace;

void host_hal_dm_board_reset(void) { memset(&host_hal_dm_board_trace, 0, sizeof(host_hal_dm_board_trace)); }

void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	if (cmn_info == ODM_CMNINFO_EXT_LNA)
		host_hal_dm_board_trace.ext_lna_2g = (u8)value;
	else if (cmn_info == ODM_CMNINFO_5G_EXT_LNA)
		host_hal_dm_board_trace.ext_lna_5g = (u8)value;
	else if (cmn_info == ODM_CMNINFO_EXT_PA)
		host_hal_dm_board_trace.ext_pa_2g = (u8)value;
	else if (cmn_info == ODM_CMNINFO_5G_EXT_PA)
		host_hal_dm_board_trace.ext_pa_5g = (u8)value;
	else if (cmn_info == ODM_CMNINFO_BOARD_TYPE)
		host_hal_dm_board_trace.board_type = (u8)value;
}

struct vector {
	char name[48];
	int external_lna_2g, external_lna_5g, external_pa_2g, external_pa_5g, eeprom_bt_coexist;
	int expect_board_type, expect_ext_lna_2g, expect_ext_lna_5g, expect_ext_pa_2g, expect_ext_pa_5g;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"external_lna_2g", "external_lna_5g", "external_pa_2g",
			      "external_pa_5g", "eeprom_bt_coexist", "expect_board_type",
			      "expect_ext_lna_2g", "expect_ext_lna_5g", "expect_ext_pa_2g",
			      "expect_ext_pa_5g"};
	int *dst[] = {&v->external_lna_2g, &v->external_lna_5g, &v->external_pa_2g,
		      &v->external_pa_5g, &v->eeprom_bt_coexist, &v->expect_board_type,
		      &v->expect_ext_lna_2g, &v->expect_ext_lna_5g, &v->expect_ext_pa_2g,
		      &v->expect_ext_pa_5g};
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
	struct dm_struct dm;

	host_hal_dm_board_reset();
	rtw_rust_hal_dm_init_odm_board_type(&dm, (u8)v->external_lna_2g,
		(u8)v->external_lna_5g, (u8)v->external_pa_2g, (u8)v->external_pa_5g,
		(u8)v->eeprom_bt_coexist);
	return host_hal_dm_board_trace.board_type != (u8)v->expect_board_type ||
	       host_hal_dm_board_trace.ext_lna_2g != (u8)v->expect_ext_lna_2g ||
	       host_hal_dm_board_trace.ext_lna_5g != (u8)v->expect_ext_lna_5g ||
	       host_hal_dm_board_trace.ext_pa_2g != (u8)v->expect_ext_pa_2g ||
	       host_hal_dm_board_trace.ext_pa_5g != (u8)v->expect_ext_pa_5g;
}

int main(int argc, char **argv)
{
	struct vector vectors[16];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_board_type_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 16, parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	printf("OK %zu vectors\n", count);
	return 0;
}
