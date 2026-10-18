// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "host_hal_dm_odm_endian_hooks_types.h"
#include "host_vector_json.h"

struct host_hal_dm_odm_endian_hook_trace host_hal_dm_odm_endian_trace;

void host_hal_dm_odm_endian_reset(void)
{
	memset(&host_hal_dm_odm_endian_trace, 0, sizeof(host_hal_dm_odm_endian_trace));
}

void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	(void)cmn_info;
	(void)value;
}

void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr)
{
	(void)dm;
	if (cmn_info == ODM_CMNINFO_BAND)
		host_hal_dm_odm_endian_trace.band = ptr;
	else if (cmn_info == ODM_CMNINFO_SEC_MODE)
		host_hal_dm_odm_endian_trace.sec_mode = ptr;
	else if (cmn_info == ODM_CMNINFO_BW)
		host_hal_dm_odm_endian_trace.bw = ptr;
	else if (cmn_info == ODM_CMNINFO_NET_CLOSED)
		host_hal_dm_odm_endian_trace.net_closed = ptr;
}

struct vector {
	char name[48];
	int big_endian, band, sec_mode, bw, net_closed;
};

static void *expect_ptr(void *field, size_t sz, int be)
{
	return be ? (u8 *)field + sz - 1 : field;
}

static int run_vector(struct vector *v)
{
	struct dm_struct dm;
	struct host_hal_dm_odm_endian_fixture fx;

	memset(&fx, 0, sizeof(fx));
	fx.band = (BAND_TYPE)v->band;
	fx.sec_mode = (u32)v->sec_mode;
	fx.bw = (enum host_hal_odm_channel_width)v->bw;
	fx.net_closed = (s32)v->net_closed;
	host_hal_dm_odm_endian_reset();
	if (v->big_endian)
		rtw_rust_hal_dm_init_odm_endian_hooks_be(&dm, &fx.band, &fx.sec_mode, &fx.bw,
			&fx.net_closed);
	else
		rtw_rust_hal_dm_init_odm_endian_hooks_le(&dm, &fx.band, &fx.sec_mode, &fx.bw,
			&fx.net_closed);
	return host_hal_dm_odm_endian_trace.band !=
		       expect_ptr(&fx.band, sizeof(BAND_TYPE), v->big_endian) ||
	       host_hal_dm_odm_endian_trace.sec_mode !=
		       expect_ptr(&fx.sec_mode, sizeof(u32), v->big_endian) ||
	       host_hal_dm_odm_endian_trace.bw !=
		       expect_ptr(&fx.bw, sizeof(enum host_hal_odm_channel_width),
				  v->big_endian) ||
	       host_hal_dm_odm_endian_trace.net_closed !=
		       expect_ptr(&fx.net_closed, sizeof(s32), v->big_endian);
}

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys[] = {"big_endian", "band", "sec_mode", "bw", "net_closed"};
	int *dst[] = {&v->big_endian, &v->band, &v->sec_mode, &v->bw, &v->net_closed};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (i = 0; i < 5; i++)
		if (host_json_parse_int_in(obj, len, keys[i], dst[i]))
			return -1;
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_odm_endian_hooks_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8, parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	printf("PASS: %zu vectors from %s\n", count, path);
	return 0;
}
