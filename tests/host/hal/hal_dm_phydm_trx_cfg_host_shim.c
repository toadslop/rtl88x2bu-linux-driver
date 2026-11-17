// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-108 PHYDM TRX cfg L2 (Rust path). */

#include <stddef.h>

#include "host_hal_dm_phydm_trx_cfg_types.h"

extern u32 g_stub_txpath;
extern u32 g_stub_rxpath;
extern u8 g_stub_api_ok;

struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter)
{
	return adapter ? adapter_to_dvobj(adapter) : NULL;
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? adapter_to_phydm(adapter) : NULL;
}

#ifdef CONFIG_CTRL_TXSS_BY_TP
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
#endif /* CONFIG_CTRL_TXSS_BY_TP */

void rtw_rust_hal_dm_err_phydm_trx_cfg(void)
{
	host_hal_dm_phydm_trx_cfg_trace.err_logged = 1;
}