// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_CAL_TRIGGER_TYPES_H
#define HOST_HAL_DM_PHYDM_CAL_TRIGGER_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct host_hal_dm_cal_adapter host_hal_dm_cal_adapter;

struct host_hal_dm_pwrctrl {
	u32 ps_deny;
};

struct host_hal_dm_cal_adapter {
	struct host_hal_dm_pwrctrl pwrctrlpriv;
	struct host_hal_dm_cal_dm {
		u8 _pad;
	} odmpriv;
};

#define HOST_PS_DENY_IOCTL 8u

#define HOST_CAL_STEP_PS_DENY 1u
#define HOST_CAL_STEP_LEAVE_PS 2u
#define HOST_CAL_STEP_IQK_ALL 3u
#define HOST_CAL_STEP_PS_DENY_CANCEL 4u

#define HOST_CAL_MAX_STEPS 8u

struct host_hal_dm_phydm_cal_trigger_trace {
	u8 steps[HOST_CAL_MAX_STEPS];
	u8 step_count;
	u32 ps_deny_reason;
	u32 ps_deny_cancel_reason;
	u8 leave_ps_direct_calls;
	u8 iqk_all_calls;
};

extern struct host_hal_dm_phydm_cal_trigger_trace host_hal_dm_phydm_cal_trigger_trace;

void host_hal_dm_phydm_cal_trigger_reset(void);
void host_hal_dm_phydm_cal_trigger_record_step(u8 step);

void rtw_ps_deny(host_hal_dm_cal_adapter *adapter, u32 reason);
void rtw_ps_deny_cancel(host_hal_dm_cal_adapter *adapter, u32 reason);
u32 rtw_ps_deny_get(host_hal_dm_cal_adapter *adapter);
void LeaveAllPowerSaveModeDirect(host_hal_dm_cal_adapter *adapter);
void rtw_phydm_iqk_trigger_all(host_hal_dm_cal_adapter *adapter);

void rtw_hal_phydm_cal_trigger(host_hal_dm_cal_adapter *adapter);

#endif