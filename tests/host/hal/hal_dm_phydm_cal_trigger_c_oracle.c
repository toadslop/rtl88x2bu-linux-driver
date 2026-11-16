// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-101 — mirrors hal/hal_dm.c rtw_hal_phydm_cal_trigger. */

#include "host_hal_dm_phydm_cal_trigger_types.h"

void rtw_hal_phydm_cal_trigger(host_hal_dm_cal_adapter *adapter)
{
	(void)adapter;

	rtw_ps_deny(adapter, HOST_PS_DENY_IOCTL);
	LeaveAllPowerSaveModeDirect(adapter);
	rtw_phydm_iqk_trigger_all(adapter);
	rtw_ps_deny_cancel(adapter, HOST_PS_DENY_IOCTL);
}