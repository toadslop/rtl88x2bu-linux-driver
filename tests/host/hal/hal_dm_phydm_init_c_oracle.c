// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-113 — mirrors hal/hal_dm.c PHYDM init orchestration. */

#include "host_hal_dm_phydm_init_types.h"

static void init_phydm_info(_adapter *adapter)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	struct dm_struct *phydm = &(hal_data->odmpriv);

	odm_cmn_info_init(phydm, ODM_CMNINFO_FW_VER, hal_data->firmware_version);
	odm_cmn_info_init(phydm, ODM_CMNINFO_FW_SUB_VER,
			  hal_data->firmware_sub_version);
}

static u8 rtw_phydm_config_trx_path(_adapter *adapter)
{
	u8 rst = _SUCCESS;

#if defined(CONFIG_RTL8192F) || defined(CONFIG_RTL8822B) || \
	defined(CONFIG_RTL8822C)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	u32 txpath = hal_data->txpath;
	u32 rxpath = hal_data->rxpath;
	u32 txpath_1ss = hal_data->txpath_nss[0];

	if (phydm_api_trx_mode(adapter_to_phydm(adapter), txpath, rxpath,
			       txpath_1ss) == _FALSE) {
		RTW_ERR("%s txpath=0x%x, rxpath=0x%x, txpath_1ss=0x%x fail\n",
			__func__, txpath, rxpath, txpath_1ss);
		rtw_warn_on(1);
		rst = _FAIL;
	}
}
#elif defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8192E)
{
	(void)adapter;
}
#endif

	return rst;
}

void rtw_phydm_init(_adapter *adapter)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	struct dm_struct *phydm = &(hal_data->odmpriv);

	phydm->adapter = adapter;
	rtw_phydm_config_trx_path(adapter);
	init_phydm_info(adapter);
	hal_data->phydm_init_result = (u8)odm_dm_init(phydm);
}