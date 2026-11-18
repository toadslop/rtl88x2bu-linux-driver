// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-111 (hal/hal_dm.c). */

#include "host_hal_dm_dyn_soml_types.h"

#ifndef RUST_HAL_DM_DYN_SOML_ORACLE

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

#endif /* !RUST_HAL_DM_DYN_SOML_ORACLE */