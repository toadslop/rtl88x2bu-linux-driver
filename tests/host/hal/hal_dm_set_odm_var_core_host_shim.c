// SPDX-License-Identifier: GPL-2.0
/* Host shims for Rust W4-94 SetHalODMVar core oracle. */

#include "host_hal_dm_set_odm_var_core_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

u8 rtw_rust_hal_dm_sta_mac_id(struct sta_info *psta)
{
	return psta->cmn.mac_id;
}

void rtw_rust_hal_dm_sta_set_dm_ctrl(struct sta_info *psta, u8 dm_ctrl)
{
	psta->cmn.dm_ctrl = dm_ctrl;
}

void *rtw_rust_hal_dm_sta_cmn_ptr(struct sta_info *psta)
{
	return &psta->cmn;
}

void rtw_rust_hal_dm_log_set_sta(u8 mac_id)
{
	host_hal_dm_set_odm_var_core_sta_log_set(mac_id);
}

void rtw_rust_hal_dm_log_clean_sta(u8 mac_id)
{
	host_hal_dm_set_odm_var_core_sta_log_clean(mac_id);
}

void rtw_rust_hal_dm_set_odm_var_rest(_adapter *adapter, u32 e_variable,
				    void *p_value1, BOOLEAN b_set)
{
	(void)adapter;
	(void)e_variable;
	(void)p_value1;
	(void)b_set;
}