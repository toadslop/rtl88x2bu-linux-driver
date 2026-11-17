// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-105 — mirrors hal/hal_dm.c rtw_phydm_iqk_trigger_dbg. */

#include "host_hal_dm_phydm_iqk_trigger_dbg_types.h"

void rtw_phydm_iqk_trigger_dbg(_adapter *adapter, boolean recovery,
			       boolean clear, boolean segment)
{
	struct dm_struct *p_dm_odm = adapter_to_phydm(adapter);

#if ((RTL8822B_SUPPORT == 1) || (RTL8821C_SUPPORT == 1) || \
     (RTL8814B_SUPPORT == 1) || (RTL8822C_SUPPORT == 1))
	halrf_segment_iqk_trigger(p_dm_odm, clear, segment);
#else
	halrf_iqk_trigger(p_dm_odm, recovery);
#endif
}