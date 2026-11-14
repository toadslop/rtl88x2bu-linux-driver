// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-84 phydm_priv_init + Init_ODM_ComInfo iface block. */

#include "host_hal_dm_odm_iface_types.h"

static u8 host_is_normal_chip(u32 version_id)
{
	return (version_id & 0xffu) == NORMAL_CHIP ? 1u : 0u;
}

static u8 host_get_intf_type(_adapter *adapter)
{
	return adapter ? adapter->host_intf_type : 0;
}

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

void rtw_phydm_priv_init(_adapter *adapter)
{
	struct dm_struct *phydm;

	if (!adapter)
		return;
	phydm = adapter_to_phydm(adapter);
	phydm->adapter = adapter;
	odm_cmn_info_init(phydm, ODM_CMNINFO_PLATFORM, ODM_CE);
}

void rtw_rust_hal_dm_init_odm_cominfo_head(_adapter *adapter)
{
	struct hal_data_host *hd;
	struct dm_struct *pdm;
	u64 odm_intf;

	if (!adapter)
		return;
	hd = GET_HAL_DATA(adapter);
	pdm = &hd->odmpriv;
	hd->phydm_op_mode = PHYDM_PERFORMANCE_MODE;
	host_odm_init_ic_type(adapter);
	if (host_get_intf_type(adapter) == RTW_GSPI)
		odm_intf = ODM_ITRF_SDIO;
	else
		odm_intf = host_get_intf_type(adapter);
	odm_cmn_info_init(pdm, ODM_CMNINFO_INTERFACE, odm_intf);
	odm_cmn_info_init(pdm, ODM_CMNINFO_MP_TEST_CHIP,
			  host_is_normal_chip(hd->version_id));
	odm_cmn_info_init(pdm, ODM_CMNINFO_PATCH_ID, hd->CustomerID);
	odm_cmn_info_init(pdm, ODM_CMNINFO_BWIFI_TEST, adapter->registrypriv.wifi_spec);
#ifdef HOST_HAL_DM_ADVANCE_OTA
	odm_cmn_info_init(pdm, ODM_CMNINFO_ADVANCE_OTA, adapter->registrypriv.adv_ota);
#endif
	odm_cmn_info_init(pdm, ODM_CMNINFO_RF_TYPE, hd->rf_type);
}

void rtw_rust_hal_dm_init_odm_cominfo_regulation_dfs(_adapter *adapter)
{
	(void)adapter;
	host_hal_set_odm_var_regulation(adapter);
}