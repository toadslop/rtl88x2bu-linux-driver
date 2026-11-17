// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_CRYSTAL_CAP_TYPES_H
#define HOST_HAL_DM_PHYDM_CRYSTAL_CAP_TYPES_H

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

struct host_hal_dm_phydm_crystal_cap_trace {
	u8 last_cap;
	u8 set_reg_calls;
	u8 stub_return;
};

extern struct host_hal_dm_phydm_crystal_cap_trace host_hal_dm_phydm_crystal_cap_trace;

void host_hal_dm_phydm_crystal_cap_reset(void);

boolean phydm_set_crystal_cap_reg(void *dm_void, u8 crystal_cap);

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);

boolean rtw_phydm_set_crystal_cap(_adapter *adapter, u8 crystal_cap);

#endif