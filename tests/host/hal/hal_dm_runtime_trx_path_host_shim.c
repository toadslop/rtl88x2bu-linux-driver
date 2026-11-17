// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-103 runtime TRX path L2. */

#include <string.h>

#include "host_hal_dm_runtime_trx_path_types.h"

struct host_hal_dm_runtime_trx_trace host_hal_dm_runtime_trx_trace;

static u32 g_stub_txpath;
static u32 g_stub_rxpath;

void host_hal_dm_runtime_trx_reset(void)
{
	memset(&host_hal_dm_runtime_trx_trace, 0,
	       sizeof(host_hal_dm_runtime_trx_trace));
	g_stub_txpath = 0;
	g_stub_rxpath = 0;
}

void host_runtime_trx_set_hal_get_paths(u32 txpath, u32 rxpath)
{
	g_stub_txpath = txpath;
	g_stub_rxpath = rxpath;
}

struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter)
{
	return adapter ? adapter_to_dvobj(adapter) : NULL;
}

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

void tx_path_nss_set_default(u32 txpath_nss[], u8 txpath_num_nss[], u8 txpath)
{
	int i, j;
	u8 cnt;

	for (i = 4; i > 0; i--) {
		cnt = 0;
		txpath_nss[i - 1] = 0;
		for (j = 0; j < RF_PATH_MAX; j++) {
			if (txpath & BIT(j)) {
				txpath_nss[i - 1] |= BIT(j);
				if (++cnt == i)
					break;
			}
		}
		txpath_num_nss[i - 1] = i;
	}
}

void dump_hal_runtime_trx_mode(void *sel, _adapter *adapter)
{
	(void)sel;
	(void)adapter;
	host_hal_dm_runtime_trx_trace.dump_calls++;
}

void rtw_warn_on(int c)
{
	if (c)
		host_hal_dm_runtime_trx_trace.warn_on_calls++;
}