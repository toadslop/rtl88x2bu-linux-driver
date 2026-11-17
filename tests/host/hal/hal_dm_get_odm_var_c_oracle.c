// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-95 GetHalODMVar (hal/hal_dm.c). */

#include "host_hal_dm_get_odm_var_types.h"

u8 rtw_phydm_get_cur_igi(_adapter *adapter)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);

	return (u8)phydm_cmn_info_query(phydm, PHYDM_INFO_CURR_IGI);
}

void GetHalODMVar(_adapter *adapter, int e_variable, void *p_value1, void *p_value2)
{
	struct dm_struct *podmpriv = adapter_to_phydm(adapter);

	(void)p_value2;
	switch (e_variable) {
#ifdef CONFIG_ANTENNA_DIVERSITY
	case HAL_ODM_ANTDIV_SELECT:
		*(u8 *)p_value1 = podmpriv->dm_fat_table.rx_idle_ant;
		break;
#endif
	case HAL_ODM_INITIAL_GAIN:
		*(u8 *)p_value1 = rtw_phydm_get_cur_igi(adapter);
		break;
	default:
		break;
	}
}