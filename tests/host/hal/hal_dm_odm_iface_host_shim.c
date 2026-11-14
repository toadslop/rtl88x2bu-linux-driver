// SPDX-License-Identifier: GPL-2.0
/* Host shims backing Rust W4-84 odm iface block under L2 test. */

#include "host_hal_dm_odm_iface_types.h"

static void host_odm_init_ic_type(_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_odm_iface_trace.ic_type_init = 1;
}

static void host_hal_set_odm_var_regulation(_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_odm_iface_trace.regulation_set = 1;
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? adapter_to_phydm(adapter) : ((struct dm_struct *)0);
}

void rtw_rust_hal_dm_phydm_set_adapter(struct dm_struct *phydm, _adapter *adapter)
{
	if (phydm)
		phydm->adapter = adapter;
}

void rtw_rust_hal_dm_set_phydm_op_mode(_adapter *adapter, u8 mode)
{
	if (adapter)
		GET_HAL_DATA(adapter)->phydm_op_mode = mode;
}

u8 rtw_rust_hal_dm_mp_test_chip_value(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return (GET_HAL_DATA(adapter)->version_id & 0xffu) == NORMAL_CHIP;
}

u32 rtw_rust_hal_dm_patch_id(_adapter *adapter)
{
	return adapter ? GET_HAL_DATA(adapter)->CustomerID : 0;
}

u8 rtw_rust_hal_dm_wifi_spec(_adapter *adapter)
{
	return adapter ? adapter->registrypriv.wifi_spec : 0;
}

u8 rtw_rust_hal_dm_rf_type(_adapter *adapter)
{
	return adapter ? GET_HAL_DATA(adapter)->rf_type : 0;
}

void rtw_rust_hal_dm_init_advance_ota_if_enabled(_adapter *adapter, struct dm_struct *phydm)
{
#ifdef HOST_HAL_DM_ADVANCE_OTA
	if (adapter && phydm)
		odm_cmn_info_init(phydm, ODM_CMNINFO_ADVANCE_OTA, adapter->registrypriv.adv_ota);
#else
	(void)adapter;
	(void)phydm;
#endif
}

void rtw_rust_hal_dm_hook_dfs_master(_adapter *adapter, struct dm_struct *phydm)
{
	(void)adapter;
	(void)phydm;
}

u8 rtw_rust_hal_dm_get_intf_type(_adapter *adapter)
{
	return adapter ? adapter->host_intf_type : 0;
}

void rtw_odm_init_ic_type(_adapter *adapter)
{
	host_odm_init_ic_type(adapter);
}

void rtw_hal_set_odm_var(_adapter *adapter, int variable, void *pvalue1, u8 bset)
{
	(void)pvalue1;
	(void)bset;
	if (adapter && variable == HAL_ODM_REGULATION)
		host_hal_set_odm_var_regulation(adapter);
}