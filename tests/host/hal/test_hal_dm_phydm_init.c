// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_phydm_init_types.h"
#include "host_vector_json.h"

struct vector {
	char name[64];
	int txpath;
	int rxpath;
	int txpath_1ss;
	int firmware_version;
	int firmware_sub_version;
	int trx_stub_ok;
	int odm_dm_init_stub;
	int expect_trx_calls;
	int expect_tx;
	int expect_rx;
	int expect_tx_1ss;
	int expect_cmn_fw_ver;
	int expect_cmn_fw_sub;
	int expect_fw_ver;
	int expect_fw_sub;
	int expect_odm_dm_init_calls;
	int expect_phydm_init_result;
	int expect_trx_warn;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	unsigned long ul;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "txpath", &ul))
		return -1;
	v->txpath = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "rxpath", &ul))
		return -1;
	v->rxpath = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "txpath_1ss", &ul))
		return -1;
	v->txpath_1ss = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "firmware_version", &ul))
		return -1;
	v->firmware_version = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "firmware_sub_version", &ul))
		return -1;
	v->firmware_sub_version = (int)ul;
	if (host_json_parse_int_in(obj, len, "trx_stub_ok", &v->trx_stub_ok))
		return -1;
	if (host_json_parse_int_in(obj, len, "odm_dm_init_stub", &v->odm_dm_init_stub))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_trx_calls", &v->expect_trx_calls))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_tx", &ul))
		return -1;
	v->expect_tx = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "expect_rx", &ul))
		return -1;
	v->expect_rx = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "expect_tx_1ss", &ul))
		return -1;
	v->expect_tx_1ss = (int)ul;
	if (host_json_parse_int_in(obj, len, "expect_cmn_fw_ver", &v->expect_cmn_fw_ver))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_cmn_fw_sub", &v->expect_cmn_fw_sub))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_fw_ver", &ul))
		return -1;
	v->expect_fw_ver = (int)ul;
	if (host_json_parse_ulong_in(obj, len, "expect_fw_sub", &ul))
		return -1;
	v->expect_fw_sub = (int)ul;
	if (host_json_parse_int_in(obj, len, "expect_odm_dm_init_calls",
				  &v->expect_odm_dm_init_calls))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_phydm_init_result",
				  &v->expect_phydm_init_result))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_trx_warn", &v->expect_trx_warn))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_init_trace *t = &host_hal_dm_phydm_init_trace;

	host_hal_dm_phydm_init_reset();
	memset(&adapter, 0, sizeof(adapter));
	adapter.HalData.txpath = (u32)v->txpath;
	adapter.HalData.rxpath = (u32)v->rxpath;
	adapter.HalData.txpath_nss[0] = (u32)v->txpath_1ss;
	adapter.HalData.firmware_version = (u8)v->firmware_version;
	adapter.HalData.firmware_sub_version = (u8)v->firmware_sub_version;
	t->trx_stub_ok = (u8)v->trx_stub_ok;
	t->odm_dm_init_stub = (u8)v->odm_dm_init_stub;

	rtw_phydm_init(&adapter);

	if (t->trx_calls != (u8)v->expect_trx_calls)
		return -1;
	if (v->expect_trx_calls && t->last_tx != (u32)v->expect_tx)
		return -1;
	if (v->expect_trx_calls && t->last_rx != (u32)v->expect_rx)
		return -1;
	if (v->expect_trx_calls && t->last_tx_1ss != (u32)v->expect_tx_1ss)
		return -1;
	if (t->cmn_fw_ver_calls != (u8)v->expect_cmn_fw_ver)
		return -1;
	if (t->cmn_fw_sub_calls != (u8)v->expect_cmn_fw_sub)
		return -1;
	if (v->expect_cmn_fw_ver && t->last_fw_ver != (u64)v->expect_fw_ver)
		return -1;
	if (v->expect_cmn_fw_sub && t->last_fw_sub != (u64)v->expect_fw_sub)
		return -1;
	if (t->odm_dm_init_calls != (u8)v->expect_odm_dm_init_calls)
		return -1;
	if (adapter.HalData.phydm_init_result != (u8)v->expect_phydm_init_result)
		return -1;
	if (t->trx_warn != (u8)v->expect_trx_warn)
		return -1;

	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1] : "hal_dm_phydm_init_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}