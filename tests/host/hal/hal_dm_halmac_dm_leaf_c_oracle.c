// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-109 — mirrors hal/hal_dm.c HALMAC DM leaf delegates. */

#include "host_hal_dm_halmac_dm_leaf_types.h"

boolean rtw_phydm_rfe_ctrl_gpio(_adapter *adapter, u8 gpio_num)
{
#ifdef RTW_HALMAC
	if (rtw_halmac_rfe_ctrl_cfg(adapter_to_dvobj(adapter), gpio_num))
		return _TRUE;
	else
#endif /* RTW_HALMAC */
		return _FALSE;
}

enum hal_status rtw_phydm_fw_iqk(struct dm_struct *p_dm_odm, u8 clear, u8 segment)
{
#ifdef RTW_HALMAC
	struct _adapter *adapter = p_dm_odm->adapter;

	if (rtw_halmac_iqk(adapter_to_dvobj(adapter), clear, segment) == 0)
		return HAL_STATUS_SUCCESS;
#endif /* RTW_HALMAC */
	return HAL_STATUS_FAILURE;
}