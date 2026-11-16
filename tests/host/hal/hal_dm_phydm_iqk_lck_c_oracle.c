// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-98 IQK/LCK RFK triggers (hal/hal_dm.c). */

#include "host_hal_dm_phydm_iqk_lck_types.h"

#if ((RTL8822B_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) || (RTL8814B_SUPPORT == 1) \
	|| (RTL8822C_SUPPORT == 1) || (RTL8723F_SUPPORT == 1))
static void iqk_segment_path(struct dm_struct *p_dm_odm)
{
	u8 clear = _TRUE, segment = _FALSE, rfk_forbidden = _FALSE;

	halrf_cmn_info_set(p_dm_odm, HALRF_CMNINFO_RFK_FORBIDDEN, rfk_forbidden);
#if (RTL8822C_SUPPORT == 1) || (RTL8814B_SUPPORT == 1) || (RTL8723F_SUPPORT == 1)
	halrf_rf_k_connect_trigger(p_dm_odm, _TRUE, SEGMENT_FREE);
#else
	halrf_cmn_info_set(p_dm_odm, HALRF_CMNINFO_IQK_SEGMENT, segment);
	halrf_segment_iqk_trigger(p_dm_odm, clear, segment);
#endif
}

void rtw_phydm_iqk_trigger(_adapter *adapter)
{
	iqk_segment_path(adapter_to_phydm(adapter));
}
#endif

void rtw_phydm_iqk_trigger_all(_adapter *adapter)
{
	struct dm_struct *p_dm_odm = adapter_to_phydm(adapter);

#if ((RTL8822B_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) || (RTL8814B_SUPPORT == 1) \
	|| (RTL8822C_SUPPORT == 1) || (RTL8723F_SUPPORT == 1))
	iqk_segment_path(p_dm_odm);
#else
	halrf_iqk_trigger(p_dm_odm, _FALSE);
#endif
}

void rtw_phydm_lck_trigger(_adapter *adapter)
{
	halrf_lck_trigger(adapter_to_phydm(adapter));
}