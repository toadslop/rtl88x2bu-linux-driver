// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_IQK_LCK_TYPES_H
#define HOST_HAL_DM_PHYDM_IQK_LCK_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t boolean;

#define _TRUE 1u
#define _FALSE 0u

/* hal/phydm/halrf/halrf.h enum halrf_cmninfo_init */
#define HALRF_CMNINFO_RFK_FORBIDDEN 3u
#define HALRF_CMNINFO_IQK_SEGMENT 4u

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

struct host_hal_dm_phydm_iqk_lck_trace {
	u32 halrf_cmn_set_info[8];
	u64 halrf_cmn_set_value[8];
	u8 halrf_cmn_set_count;
	u8 segment_clear;
	u8 segment_flag;
	u8 segment_calls;
	u8 lck_calls;
	u8 plain_iqk_recovery;
	u8 plain_iqk_calls;
};

extern struct host_hal_dm_phydm_iqk_lck_trace host_hal_dm_phydm_iqk_lck_trace;

void host_hal_dm_phydm_iqk_lck_reset(void);

void halrf_cmn_info_set(void *dm_void, u32 cmn_info, u64 value);
void halrf_segment_iqk_trigger(void *dm_void, boolean clear, boolean segment);
void halrf_iqk_trigger(void *dm_void, boolean is_recovery);
void halrf_lck_trigger(void *dm_void);

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);

void rtw_phydm_iqk_trigger(_adapter *adapter);
void rtw_phydm_iqk_trigger_all(_adapter *adapter);
void rtw_phydm_lck_trigger(_adapter *adapter);

#endif