// SPDX-License-Identifier: GPL-2.0
/* C oracle + PHYDM/hwreg mocks for W4-111 (hal/hal_dm.c). */

#include <string.h>

#include "host_hal_dm_dyn_soml_types.h"

struct host_hal_dm_dyn_soml_trace host_hal_dm_dyn_soml_trace;

void host_hal_dm_dyn_soml_reset(void)
{
	memset(&host_hal_dm_dyn_soml_trace, 0, sizeof(host_hal_dm_dyn_soml_trace));
}

void phydm_soml_bytes_acq(void *dm_void, u8 rate_id, u32 length)
{
	(void)dm_void;
	host_hal_dm_dyn_soml_trace.bytes_acq_calls++;
	host_hal_dm_dyn_soml_trace.bytes_acq_rate = rate_id;
	host_hal_dm_dyn_soml_trace.bytes_acq_length = length;
}

void phydm_adaptive_soml_para_set(void *dm_void, u8 train_num, u8 intvl,
				  u8 period, u8 delay)
{
	(void)dm_void;
	host_hal_dm_dyn_soml_trace.para_set_calls++;
	host_hal_dm_dyn_soml_trace.para_train = train_num;
	host_hal_dm_dyn_soml_trace.para_intvl = intvl;
	host_hal_dm_dyn_soml_trace.para_period = period;
	host_hal_dm_dyn_soml_trace.para_delay = delay;
}

void rtw_hal_set_hwreg(_adapter *adapter, enum host_hw_var variable, u8 *val)
{
	(void)adapter;
	(void)val;
	host_hal_dm_dyn_soml_trace.set_hwreg_calls++;
	host_hal_dm_dyn_soml_trace.set_hwreg_var = (u32)variable;
}

void rtw_dyn_soml_byte_update(_adapter *adapter, u8 data_rate, u32 size)
{
	phydm_soml_bytes_acq(adapter_to_phydm(adapter), data_rate, size);
}

void rtw_dyn_soml_para_set(_adapter *adapter, u8 train_num, u8 intvl,
			   u8 period, u8 delay)
{
	phydm_adaptive_soml_para_set(adapter_to_phydm(adapter), train_num, intvl,
				     period, delay);
	RTW_INFO("%s.\n", __func__);
}

void rtw_dyn_soml_config(_adapter *adapter)
{
	RTW_INFO("%s.\n", __func__);

	if (adapter->registrypriv.dyn_soml_en == 1) {
		rtw_hal_set_hwreg(adapter, HOST_HW_VAR_SET_SOML_PARAM, NULL);
		RTW_INFO("dyn_soml_en = 1\n");
	} else if (adapter->registrypriv.dyn_soml_en == 2) {
		rtw_dyn_soml_para_set(adapter, adapter->registrypriv.dyn_soml_train_num,
				      adapter->registrypriv.dyn_soml_interval,
				      adapter->registrypriv.dyn_soml_period,
				      adapter->registrypriv.dyn_soml_delay);
		RTW_INFO("dyn_soml_en = 2\n");
	} else if (adapter->registrypriv.dyn_soml_en == 0) {
		RTW_INFO("dyn_soml_en = 0\n");
	} else {
		RTW_ERR("%s, wrong setting: dyn_soml_en = %d\n", __func__,
			adapter->registrypriv.dyn_soml_en);
	}
}