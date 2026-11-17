// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-106 dbg RF cal test L2 harness. */

#include "host_hal_dm_dbg_rf_cal_test_types.h"

void *rtw_rust_hal_dm_adapter_to_phydm(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	return adapter ? (void *)&adapter->odmpriv : (void *)0;
}

void rtw_ps_deny(host_hal_dm_dbg_rf_cal_adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	if (adapter->pwrctrlpriv.ps_deny & (1U << reason))
		return;
	adapter->pwrctrlpriv.ps_deny |= (1U << reason);
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_PS_DENY);
	host_hal_dm_dbg_rf_cal_trace.ps_deny_reason = reason;
}

void rtw_ps_deny_cancel(host_hal_dm_dbg_rf_cal_adapter *adapter, u32 reason)
{
	if (!adapter)
		return;
	adapter->pwrctrlpriv.ps_deny &= ~(1U << reason);
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_PS_DENY_CANCEL);
}

u32 rtw_ps_deny_get(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	if (!adapter)
		return 0;
	return adapter->pwrctrlpriv.ps_deny;
}

void LeaveAllPowerSaveModeDirect(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_LEAVE_PS);
}

void rtw_phydm_ability_backup(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_ABILITY_BACKUP);
	host_hal_dm_dbg_rf_cal_trace.ability_backup_calls++;
}

void rtw_phydm_func_disable_all(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_FUNC_DISABLE_ALL);
	host_hal_dm_dbg_rf_cal_trace.func_disable_calls++;
}

void rtw_phydm_ability_restore(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_ABILITY_RESTORE);
	host_hal_dm_dbg_rf_cal_trace.ability_restore_calls++;
}

void halrf_cmn_info_set(void *dm_void, u32 cmn_info, u64 value)
{
	(void)dm_void;
	if (cmn_info == HOST_HALRF_CMNINFO_ABILITY)
		host_hal_dm_dbg_rf_cal_trace.halrf_ability_mask = value;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_HALRF_ABILITY_SET);
}

void rtw_phydm_iqk_trigger_dbg(host_hal_dm_dbg_rf_cal_adapter *adapter, bool recovery,
			       bool clear, bool segment)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_IQK_DBG);
	host_hal_dm_dbg_rf_cal_trace.iqk_dbg_calls++;
	host_hal_dm_dbg_rf_cal_trace.last_iqk_recovery = recovery ? 1u : 0u;
	host_hal_dm_dbg_rf_cal_trace.last_iqk_clear = clear ? 1u : 0u;
	host_hal_dm_dbg_rf_cal_trace.last_iqk_segment = segment ? 1u : 0u;
}

void rtw_phydm_lck_trigger(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	(void)adapter;
	host_hal_dm_dbg_rf_cal_record_step(HOST_DBG_RF_STEP_LCK_TRIGGER);
	host_hal_dm_dbg_rf_cal_trace.lck_trigger_calls++;
}