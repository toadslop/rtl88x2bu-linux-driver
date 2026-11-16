// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-92 PHYDM IGI and IQK status queries (hal/hal_dm.c). */

#include "host_hal_dm_phydm_igi_iqk_types.h"

u8 rtw_phydm_get_cur_igi(_adapter *adapter)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u8 cur_igi = 0;

	cur_igi = (u8)phydm_cmn_info_query(phydm, PHYDM_INFO_CURR_IGI);
	return cur_igi;
}

u8 rtw_phydm_is_iqk_in_progress(_adapter *adapter)
{
	u8 rts = _FALSE;
	struct dm_struct *podmpriv = adapter_to_phydm(adapter);

	odm_acquire_spin_lock(podmpriv, RT_IQK_SPINLOCK);
	if (podmpriv->rf_calibrate_info.is_iqk_in_progress == _TRUE) {
		RTW_ERR("IQK InProgress\n");
		rts = _TRUE;
	}
	odm_release_spin_lock(podmpriv, RT_IQK_SPINLOCK);

	return rts;
}