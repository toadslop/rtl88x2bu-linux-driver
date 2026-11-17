// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host_hal_dm_runtime_trx_path_types.h"
#include "host_vector_json.h"

struct vector {
	char name[48];
	unsigned long stub_txpath;
	unsigned long stub_rxpath;
	unsigned long trx_path_bmp;
	int max_tx_cnt;
	int path_div;
	int tx_npath;
	int expect_result;
	unsigned long expect_txpath;
	unsigned long expect_txpath_nss0;
	int expect_txpath_num_nss0;
	int expect_dump_calls;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "stub_txpath", &v->stub_txpath))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "stub_rxpath", &v->stub_rxpath))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "trx_path_bmp", &v->trx_path_bmp))
		return -1;
	if (host_json_parse_int_in(obj, len, "max_tx_cnt", &v->max_tx_cnt))
		return -1;
	if (host_json_parse_int_in(obj, len, "path_div", &v->path_div))
		return -1;
	if (host_json_parse_int_in(obj, len, "tx_npath", &v->tx_npath))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_result", &v->expect_result))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_txpath", &v->expect_txpath))
		return -1;
	if (host_json_parse_ulong_in(obj, len, "expect_txpath_nss0",
				    &v->expect_txpath_nss0))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_txpath_num_nss0",
				   &v->expect_txpath_num_nss0))
		return -1;
	if (host_json_parse_int_in(obj, len, "expect_dump_calls",
				   &v->expect_dump_calls))
		return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	u8 ret;
	struct host_hal_dm_runtime_trx_trace *t = &host_hal_dm_runtime_trx_trace;

	host_hal_dm_runtime_trx_reset();
	memset(&adapter, 0, sizeof(adapter));
	adapter.HalData.trx_path_bmp = (u8)v->trx_path_bmp;
	adapter.HalData.max_tx_cnt = (u8)v->max_tx_cnt;
	adapter.registrypriv.path_div = (u8)v->path_div;
	adapter.registrypriv.tx_npath = (u8)v->tx_npath;
	host_runtime_trx_set_hal_get_paths((u32)v->stub_txpath,
					   (u32)v->stub_rxpath);

	ret = rtw_hal_runtime_trx_path_decision(&adapter);
	if (ret != (u8)v->expect_result)
		return -1;
	if (adapter.HalData.txpath != (u32)v->expect_txpath)
		return -1;
	if (ret == _SUCCESS) {
		if (adapter.HalData.txpath_nss[0] != (u32)v->expect_txpath_nss0)
			return -1;
		if (adapter.HalData.txpath_num_nss[0] !=
		    (u8)v->expect_txpath_num_nss0)
			return -1;
	}
	if (t->dump_calls != (u8)v->expect_dump_calls)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[12];
	size_t count = 0;
	const char *path = argc > 1 ? argv[1]
				    : "hal_dm_runtime_trx_path_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 12,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}