// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_IQK_TRIGGER_DBG_TYPES_H
#define HOST_HAL_DM_PHYDM_IQK_TRIGGER_DBG_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef u8 boolean;

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_phydm_iqk_trigger_dbg_trace {
	u8 segment_clear;
	u8 segment_flag;
	unsigned segment_calls;
	u8 plain_iqk_recovery;
	unsigned plain_iqk_calls;
};

extern struct host_hal_dm_phydm_iqk_trigger_dbg_trace
	host_hal_dm_phydm_iqk_trigger_dbg_trace;

void host_hal_dm_phydm_iqk_trigger_dbg_reset(void);

void halrf_segment_iqk_trigger(void *dm_void, boolean clear, boolean segment);
void halrf_iqk_trigger(void *dm_void, boolean is_recovery);

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);

void rtw_phydm_iqk_trigger_dbg(_adapter *adapter, boolean recovery,
			       boolean clear, boolean segment);

#endif