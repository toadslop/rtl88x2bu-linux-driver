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

void rtw_rust_hal_dm_runtime_trx_nss_default(_adapter *adapter)
{
	struct hal_data_host *hal = GET_HAL_DATA(adapter);

	tx_path_nss_set_default(hal->txpath_nss, hal->txpath_num_nss,
				GET_HAL_TX_PATH_BMP(adapter));
}

u8 rtw_rust_hal_dm_runtime_trx_max_tx_cnt(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->max_tx_cnt;
}

u8 rtw_rust_hal_dm_runtime_trx_path_div(_adapter *adapter)
{
	return adapter->registrypriv.path_div;
}

u8 rtw_rust_hal_dm_runtime_trx_tx_npath(_adapter *adapter)
{
	return adapter->registrypriv.tx_npath;
}

u32 rtw_rust_hal_dm_runtime_trx_get_txpath_nss0(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->txpath_nss[0];
}

void rtw_rust_hal_dm_runtime_trx_set_txpath_nss0(_adapter *adapter,
						 u32 txpath_1ss, u8 num)
{
	struct hal_data_host *hal = GET_HAL_DATA(adapter);

	hal->txpath_nss[0] = txpath_1ss;
	hal->txpath_num_nss[0] = num;
}

void rtw_rust_hal_dm_runtime_trx_set_paths(_adapter *adapter, u32 txpath,
					   u32 rxpath)
{
	struct hal_data_host *hal = GET_HAL_DATA(adapter);

	hal->txpath = txpath;
	hal->rxpath = rxpath;
}

void rtw_rust_hal_dm_runtime_trx_dump(void *sel, _adapter *adapter)
{
	dump_hal_runtime_trx_mode(sel, adapter);
}

void rtw_rust_hal_dm_runtime_trx_log_empty_tx(void)
{
	rtw_warn_on(1);
}

void rtw_rust_hal_dm_runtime_trx_log_empty_rx(void)
{
	rtw_warn_on(1);
}