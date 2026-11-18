/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_DYN_SOML_TYPES_H
#define HOST_HAL_DM_DYN_SOML_TYPES_H

#include "host_types.h"

enum host_hw_var {
	HOST_HW_VAR_SET_SOML_PARAM = 209,
};

struct dm_struct {
	u8 _pad;
};

struct registry_host {
	u8 dyn_soml_en;
	u8 dyn_soml_train_num;
	u8 dyn_soml_interval;
	u8 dyn_soml_period;
	u8 dyn_soml_delay;
};

typedef struct {
	struct dm_struct odmpriv;
	struct registry_host registrypriv;
} _adapter;

#define adapter_to_phydm(a) (&(a)->odmpriv)

struct host_hal_dm_dyn_soml_trace {
	u8 bytes_acq_calls;
	u8 bytes_acq_rate;
	u32 bytes_acq_length;
	u8 para_set_calls;
	u8 para_train;
	u8 para_intvl;
	u8 para_period;
	u8 para_delay;
	u8 set_hwreg_calls;
	u32 set_hwreg_var;
	u8 err_logged;
};

extern struct host_hal_dm_dyn_soml_trace host_hal_dm_dyn_soml_trace;

void host_hal_dm_dyn_soml_reset(void);
void rtw_dyn_soml_byte_update(_adapter *a, u8 rate, u32 size);
void rtw_dyn_soml_para_set(_adapter *a, u8 tr, u8 iv, u8 pe, u8 de);
void rtw_dyn_soml_config(_adapter *a);

#define RTW_INFO(...) ((void)0)
#define RTW_ERR(...) (host_hal_dm_dyn_soml_trace.err_logged = 1)

#endif