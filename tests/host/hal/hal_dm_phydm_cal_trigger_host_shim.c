// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-101 PHYDM cal trigger L2 harness. */

#include "host_hal_dm_phydm_cal_trigger_types.h"

void rtw_ps_deny(host_hal_dm_cal_adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	if (adapter->pwrctrlpriv.ps_deny & (1U << reason))
		return;
	adapter->pwrctrlpriv.ps_deny |= (1U << reason);
	host_hal_dm_phydm_cal_trigger_record_step(HOST_CAL_STEP_PS_DENY);
	host_hal_dm_phydm_cal_trigger_trace.ps_deny_reason = reason;
}

void rtw_ps_deny_cancel(host_hal_dm_cal_adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	adapter->pwrctrlpriv.ps_deny &= ~(1U << reason);
	host_hal_dm_phydm_cal_trigger_record_step(HOST_CAL_STEP_PS_DENY_CANCEL);
	host_hal_dm_phydm_cal_trigger_trace.ps_deny_cancel_reason = reason;
}

u32 rtw_ps_deny_get(host_hal_dm_cal_adapter *adapter)
{
	if (!adapter)
		return 0;
	return adapter->pwrctrlpriv.ps_deny;
}

void LeaveAllPowerSaveModeDirect(host_hal_dm_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_phydm_cal_trigger_record_step(HOST_CAL_STEP_LEAVE_PS);
	host_hal_dm_phydm_cal_trigger_trace.leave_ps_direct_calls++;
}

void rtw_phydm_iqk_trigger_all(host_hal_dm_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_phydm_cal_trigger_record_step(HOST_CAL_STEP_IQK_ALL);
	host_hal_dm_phydm_cal_trigger_trace.iqk_all_calls++;
}