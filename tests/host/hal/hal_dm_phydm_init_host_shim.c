// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-113 PHYDM init orchestration L2. */

#include "host_hal_dm_phydm_init_types.h"

struct host_hal_dm_phydm_init_trace host_hal_dm_phydm_init_trace;

void host_hal_dm_phydm_init_reset(void)
{
	memset(&host_hal_dm_phydm_init_trace, 0,
	       sizeof(host_hal_dm_phydm_init_trace));
}

boolean phydm_api_trx_mode(void *dm_void, u32 tx_path, u32 rx_path,
			   u32 tx_path_ctrl)
{
	(void)dm_void;
	host_hal_dm_phydm_init_trace.trx_calls++;
	host_hal_dm_phydm_init_trace.last_tx = tx_path;
	host_hal_dm_phydm_init_trace.last_rx = rx_path;
	host_hal_dm_phydm_init_trace.last_tx_1ss = tx_path_ctrl;
	return host_hal_dm_phydm_init_trace.trx_stub_ok ? _TRUE : _FALSE;
}

void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	if (cmn_info == ODM_CMNINFO_FW_VER) {
		host_hal_dm_phydm_init_trace.cmn_fw_ver_calls++;
		host_hal_dm_phydm_init_trace.last_fw_ver = value;
	} else if (cmn_info == ODM_CMNINFO_FW_SUB_VER) {
		host_hal_dm_phydm_init_trace.cmn_fw_sub_calls++;
		host_hal_dm_phydm_init_trace.last_fw_sub = value;
	}
}

enum phydm_init_result_host odm_dm_init(struct dm_struct *dm)
{
	(void)dm;
	host_hal_dm_phydm_init_trace.odm_dm_init_calls++;
	return (enum phydm_init_result_host)
		host_hal_dm_phydm_init_trace.odm_dm_init_stub;
}

void rtw_warn_on(int cond)
{
	(void)cond;
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? adapter_to_phydm(adapter) : NULL;
}

struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter)
{
	return adapter ? adapter_to_dvobj(adapter) : NULL;
}

u32 rtw_rust_hal_dm_phydm_init_txpath(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->txpath;
}

u32 rtw_rust_hal_dm_phydm_init_rxpath(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->rxpath;
}

u32 rtw_rust_hal_dm_phydm_init_txpath_1ss(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->txpath_nss[0];
}

u8 rtw_rust_hal_dm_phydm_init_fw_ver(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->firmware_version;
}

u8 rtw_rust_hal_dm_phydm_init_fw_sub_ver(_adapter *adapter)
{
	return GET_HAL_DATA(adapter)->firmware_sub_version;
}

void rtw_rust_hal_dm_phydm_init_set_result(_adapter *adapter, u8 result)
{
	GET_HAL_DATA(adapter)->phydm_init_result = result;
}

void rtw_rust_hal_dm_phydm_init_err_trx_config(const char *func, u32 txpath,
					       u32 rxpath, u32 txpath_1ss)
{
	(void)func;
	(void)txpath;
	(void)rxpath;
	(void)txpath_1ss;
	host_hal_dm_phydm_init_trace.trx_warn = 1;
}