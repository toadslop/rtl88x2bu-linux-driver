// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_PHYDM_RRSR_TYPES_H
#define HOST_HAL_DM_PHYDM_RRSR_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef u8 boolean;

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

struct registry_host {
	u32 set_rrsr_value;
};

typedef struct {
	struct hal_data_host HalData;
	struct registry_host registrypriv;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

#define ODM_CMNINFO_RRSR_VAL 111u

struct host_hal_dm_phydm_rrsr_trace {
	u32 cmn_update_info;
	u64 cmn_update_value;
	u8 cmn_update_calls;
	u32 reg_set_value;
	u8 reg_set_calls;
	u8 rrsr_en;
	u8 rrsr_en_calls;
};

extern struct host_hal_dm_phydm_rrsr_trace host_hal_dm_phydm_rrsr_trace;

void host_hal_dm_phydm_rrsr_reset(void);

void odm_cmn_info_update(struct dm_struct *dm, u32 cmn_info, u64 value);
void phydm_rrsr_set_register(void *dm_void, u32 rrsr_val);
void phydm_rrsr_en(void *dm_void, boolean en_rrsr);

void rtw_phydm_set_rrsr(_adapter *adapter, u32 rrsr_value, boolean write_rrsr);
void rtw_phydm_dyn_rrsr_en(_adapter *adapter, boolean en_rrsr);

#endif