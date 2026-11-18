// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_READ_EFUSE_TRIM_TYPES_H
#define HOST_HAL_DM_PHYDM_READ_EFUSE_TRIM_TYPES_H

#include <stdint.h>

typedef uint8_t u8;

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

struct host_hal_dm_phydm_read_efuse_trim_trace {
	unsigned thermal_trim_calls;
	unsigned power_trim_calls;
};

extern struct host_hal_dm_phydm_read_efuse_trim_trace
	host_hal_dm_phydm_read_efuse_trim_trace;

void host_hal_dm_phydm_read_efuse_trim_reset(void);

void phydm_get_thermal_trim_offset(void *dm_void);
void phydm_get_power_trim_offset(void *dm_void);

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);

void rtw_phydm_read_efuse(_adapter *adapter);

#endif