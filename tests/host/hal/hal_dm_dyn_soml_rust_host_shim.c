// SPDX-License-Identifier: GPL-2.0
/* Rust L2 shims for W4-111 (PHydm + registry FFI). */

#include <string.h>

#include "host_hal_dm_dyn_soml_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

void rtw_rust_hal_dm_dyn_soml_set_hwreg(_adapter *adapter)
{
	rtw_hal_set_hwreg(adapter, HOST_HW_VAR_SET_SOML_PARAM, NULL);
}

u8 rtw_rust_hal_dm_registry_dyn_soml_en(_adapter *adapter)
{
	return adapter ? adapter->registrypriv.dyn_soml_en : 0;
}

u8 rtw_rust_hal_dm_registry_dyn_soml_train_num(_adapter *adapter)
{
	return adapter->registrypriv.dyn_soml_train_num;
}

u8 rtw_rust_hal_dm_registry_dyn_soml_interval(_adapter *adapter)
{
	return adapter->registrypriv.dyn_soml_interval;
}

u8 rtw_rust_hal_dm_registry_dyn_soml_period(_adapter *adapter)
{
	return adapter->registrypriv.dyn_soml_period;
}

u8 rtw_rust_hal_dm_registry_dyn_soml_delay(_adapter *adapter)
{
	return adapter->registrypriv.dyn_soml_delay;
}

void rtw_rust_hal_dm_log_dyn_soml_config(void) {}
void rtw_rust_hal_dm_log_dyn_soml_para_set(void) {}
void rtw_rust_hal_dm_log_dyn_soml_en(u8 en) { (void)en; }

void rtw_rust_hal_dm_log_dyn_soml_en2_params(u8 a, u8 b, u8 c, u8 d)
{
	(void)a;
	(void)b;
	(void)c;
	(void)d;
}

void rtw_rust_hal_dm_err_dyn_soml_bad_en(_adapter *adapter, u8 en)
{
	(void)adapter;
	(void)en;
	host_hal_dm_dyn_soml_trace.err_logged = 1;
}