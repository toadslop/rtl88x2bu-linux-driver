// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-106 — mirrors hal/hal_dm.c dbg RF cal test wrappers. */

#include "host_hal_dm_dbg_rf_cal_test_types.h"

void rtw_hal_iqk_test(host_hal_dm_dbg_rf_cal_adapter *adapter, bool recovery, bool clear,
		      bool segment)
{
	void *p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
	(void)p_dm_odm;

	rtw_ps_deny(adapter, HOST_PS_DENY_IOCTL);
	LeaveAllPowerSaveModeDirect(adapter);

	rtw_phydm_ability_backup(adapter);
	rtw_phydm_func_disable_all(adapter);

	halrf_cmn_info_set(p_dm_odm, HOST_HALRF_CMNINFO_ABILITY, HOST_HAL_RF_IQK);

	rtw_phydm_iqk_trigger_dbg(adapter, recovery, clear, segment);
	rtw_phydm_ability_restore(adapter);

	rtw_ps_deny_cancel(adapter, HOST_PS_DENY_IOCTL);
}

void rtw_hal_lck_test(host_hal_dm_dbg_rf_cal_adapter *adapter)
{
	void *p_dm_odm = rtw_rust_hal_dm_adapter_to_phydm(adapter);
	(void)p_dm_odm;

	rtw_ps_deny(adapter, HOST_PS_DENY_IOCTL);
	LeaveAllPowerSaveModeDirect(adapter);

	rtw_phydm_ability_backup(adapter);
	rtw_phydm_func_disable_all(adapter);

	halrf_cmn_info_set(p_dm_odm, HOST_HALRF_CMNINFO_ABILITY, HOST_HAL_RF_LCK);

	rtw_phydm_lck_trigger(adapter);

	rtw_phydm_ability_restore(adapter);
	rtw_ps_deny_cancel(adapter, HOST_PS_DENY_IOCTL);
}