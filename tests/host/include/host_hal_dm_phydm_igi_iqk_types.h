// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_IGI_IQK_TYPES_H
#define HOST_HAL_DM_PHYDM_IGI_IQK_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint8_t boolean;

#define PHYDM_INFO_CURR_IGI 20u /* phydm.h enum phydm_info_query */
#define RT_IQK_SPINLOCK 38u

#define _TRUE 1u
#define _FALSE 0u

struct dm_rf_calibrate_host {
	u8 is_iqk_in_progress;
};

struct dm_struct {
	struct dm_rf_calibrate_host rf_calibrate_info;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct {
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_phydm_igi_iqk_trace {
	u32 phydm_cmn_query_last;
	u32 phydm_cmn_query_ret;
	u8 iqk_lock_acquired;
	u8 iqk_lock_released;
	u8 err_logged;
};

extern struct host_hal_dm_phydm_igi_iqk_trace host_hal_dm_phydm_igi_iqk_trace;

void host_hal_dm_phydm_igi_iqk_reset(void);
void host_hal_dm_phydm_igi_iqk_trace_err(void);

#define RTW_ERR(fmt, ...) host_hal_dm_phydm_igi_iqk_trace_err()

u32 phydm_cmn_info_query(struct dm_struct *dm, u32 info_type);
void odm_acquire_spin_lock(struct dm_struct *dm, u32 lock_type);
void odm_release_spin_lock(struct dm_struct *dm, u32 lock_type);

u8 rtw_phydm_get_cur_igi(_adapter *adapter);
u8 rtw_phydm_is_iqk_in_progress(_adapter *adapter);

#endif