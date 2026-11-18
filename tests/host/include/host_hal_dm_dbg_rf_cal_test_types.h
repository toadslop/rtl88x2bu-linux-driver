// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_DBG_RF_CAL_TEST_TYPES_H
#define HOST_HAL_DM_DBG_RF_CAL_TEST_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct host_hal_dm_dbg_rf_cal_adapter host_hal_dm_dbg_rf_cal_adapter;

struct host_hal_dm_dbg_rf_cal_adapter {
	struct { u32 ps_deny; } pwrctrlpriv;
	struct { u8 _pad; } odmpriv;
};

#define HOST_PS_DENY_IOCTL 5u
#define HOST_HALRF_CMNINFO_ABILITY 0u
#define HOST_HAL_RF_IQK (1u << 1)
#define HOST_HAL_RF_LCK (1u << 2)

#define HOST_DBG_RF_STEP_PS_DENY 1u
#define HOST_DBG_RF_STEP_LEAVE_PS 2u
#define HOST_DBG_RF_STEP_ABILITY_BACKUP 3u
#define HOST_DBG_RF_STEP_FUNC_DISABLE_ALL 4u
#define HOST_DBG_RF_STEP_HALRF_ABILITY_SET 5u
#define HOST_DBG_RF_STEP_IQK_DBG 6u
#define HOST_DBG_RF_STEP_LCK_TRIGGER 7u
#define HOST_DBG_RF_STEP_ABILITY_RESTORE 8u
#define HOST_DBG_RF_STEP_PS_DENY_CANCEL 9u
#define HOST_DBG_RF_MAX_STEPS 12u

struct host_hal_dm_dbg_rf_cal_trace {
	u8 steps[HOST_DBG_RF_MAX_STEPS];
	u8 step_count;
	u32 ps_deny_reason;
	u64 halrf_ability_mask;
	u8 iqk_dbg_calls;
	u8 lck_trigger_calls;
	u8 ability_backup_calls;
	u8 func_disable_calls;
	u8 ability_restore_calls;
};

extern struct host_hal_dm_dbg_rf_cal_trace host_hal_dm_dbg_rf_cal_trace;

void host_hal_dm_dbg_rf_cal_reset(void);
void host_hal_dm_dbg_rf_cal_record_step(u8 step);

void *rtw_rust_hal_dm_adapter_to_phydm(host_hal_dm_dbg_rf_cal_adapter *adapter);
void rtw_ps_deny(host_hal_dm_dbg_rf_cal_adapter *adapter, u32 reason);
void rtw_ps_deny_cancel(host_hal_dm_dbg_rf_cal_adapter *adapter, u32 reason);
u32 rtw_ps_deny_get(host_hal_dm_dbg_rf_cal_adapter *adapter);
void LeaveAllPowerSaveModeDirect(host_hal_dm_dbg_rf_cal_adapter *adapter);
void rtw_phydm_ability_backup(host_hal_dm_dbg_rf_cal_adapter *adapter);
void rtw_phydm_func_disable_all(host_hal_dm_dbg_rf_cal_adapter *adapter);
void rtw_phydm_ability_restore(host_hal_dm_dbg_rf_cal_adapter *adapter);
void halrf_cmn_info_set(void *dm_void, u32 cmn_info, u64 value);
void rtw_phydm_iqk_trigger_dbg(host_hal_dm_dbg_rf_cal_adapter *adapter, bool recovery,
			       bool clear, bool segment);
void rtw_phydm_lck_trigger(host_hal_dm_dbg_rf_cal_adapter *adapter);
void rtw_hal_iqk_test(host_hal_dm_dbg_rf_cal_adapter *adapter, bool recovery, bool clear,
		      bool segment);
void rtw_hal_lck_test(host_hal_dm_dbg_rf_cal_adapter *adapter);

#endif