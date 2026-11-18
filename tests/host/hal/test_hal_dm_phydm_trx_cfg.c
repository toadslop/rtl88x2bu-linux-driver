// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_hal_dm_phydm_trx_cfg_types.h"
#include "host_vector_json.h"

struct host_hal_dm_phydm_trx_cfg_trace host_hal_dm_phydm_trx_cfg_trace;

u32 g_stub_txpath;
u32 g_stub_rxpath;
u8 g_stub_api_ok = 1;

void host_hal_dm_phydm_trx_cfg_reset(void)
{
	memset(&host_hal_dm_phydm_trx_cfg_trace, 0,
	       sizeof(host_hal_dm_phydm_trx_cfg_trace));
	g_stub_txpath = 0;
	g_stub_rxpath = 0;
	g_stub_api_ok = 1;
}

void host_trx_cfg_set_hal_paths(u32 txpath, u32 rxpath)
{
	g_stub_txpath = txpath;
	g_stub_rxpath = rxpath;
}

void host_trx_cfg_set_api_return(u8 ok)
{
	g_stub_api_ok = ok;
}

#if defined(CONFIG_CTRL_TXSS_BY_TP) && !defined(RUST_HAL_DM_PHYDM_TRX_CFG_ORACLE)
void rtw_hal_get_trx_path(struct dvobj_host *d, void *type, u32 *txpath,
			  u32 *rxpath)
{
	(void)d;
	(void)type;
	if (txpath)
		*txpath = g_stub_txpath;
	if (rxpath)
		*rxpath = g_stub_rxpath;
}

boolean phydm_api_trx_mode(void *dm_void, u32 tx_path, u32 rx_path,
			   u32 tx_path_ctrl)
{
	(void)dm_void;
	host_hal_dm_phydm_trx_cfg_trace.api_calls++;
	host_hal_dm_phydm_trx_cfg_trace.last_tx = tx_path;
	host_hal_dm_phydm_trx_cfg_trace.last_rx = rx_path;
	host_hal_dm_phydm_trx_cfg_trace.last_tx_1ss = tx_path_ctrl;
	return g_stub_api_ok ? _TRUE : _FALSE;
}
#endif /* CONFIG_CTRL_TXSS_BY_TP && !RUST */

struct vector {
	char name[48];
	int tx_1ss;
	int stub_tx;
	int stub_rx;
	int api_ok;
	int expect_tx;
	int expect_rx;
	int expect_tx_1ss;
	int expect_err;
};

static int parse_vector_object(const char *obj, size_t len, void *vv)
{
	struct vector *v = vv;
	const char *keys_i[] = {"tx_1ss", "stub_tx", "stub_rx", "api_ok", "expect_tx",
				"expect_rx", "expect_tx_1ss", "expect_err"};
	int *dst[] = {&v->tx_1ss, &v->stub_tx, &v->stub_rx, &v->api_ok, &v->expect_tx,
		      &v->expect_rx, &v->expect_tx_1ss, &v->expect_err};
	size_t i;

	memset(v, 0, sizeof(*v));
	if (host_json_parse_string_in(obj, len, "name", v->name, sizeof(v->name)))
		return -1;
	for (i = 0; i < sizeof(keys_i) / sizeof(keys_i[0]); i++)
		if (host_json_parse_int_in(obj, len, keys_i[i], dst[i]))
			return -1;
	return 0;
}

static int run_vector(struct vector *v)
{
	_adapter adapter;
	struct host_hal_dm_phydm_trx_cfg_trace *t = &host_hal_dm_phydm_trx_cfg_trace;

	host_hal_dm_phydm_trx_cfg_reset();
	memset(&adapter, 0, sizeof(adapter));
	host_trx_cfg_set_hal_paths((u32)v->stub_tx, (u32)v->stub_rx);
	host_trx_cfg_set_api_return((u8)v->api_ok);

	rtw_phydm_trx_cfg(&adapter, v->tx_1ss != 0);
	if (t->api_calls != 1)
		return -1;
	if (t->last_tx != (u32)v->expect_tx || t->last_rx != (u32)v->expect_rx ||
	    t->last_tx_1ss != (u32)v->expect_tx_1ss)
		return -1;
	if (!!t->err_logged != !!v->expect_err)
		return -1;
	printf("PASS %s\n", v->name);
	return 0;
}

int main(int argc, char **argv)
{
	struct vector vectors[8];
	size_t count = 0;
	const char *path =
		argc > 1 ? argv[1] : "hal_dm_phydm_trx_cfg_vectors.json";

	if (host_load_vectors(path, vectors, sizeof(vectors[0]), 8,
			      parse_vector_object, &count))
		return fprintf(stderr, "failed to load %s\n", path), 1;
	for (size_t i = 0; i < count; i++)
		if (run_vector(&vectors[i]))
			return fprintf(stderr, "FAIL %s\n", vectors[i].name), 1;
	return 0;
}