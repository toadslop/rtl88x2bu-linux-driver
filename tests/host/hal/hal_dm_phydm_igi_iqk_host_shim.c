// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-92 PHYDM IGI / IQK queries under L2 Rust test. */

#include "host_hal_dm_phydm_igi_iqk_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? adapter_to_phydm(adapter) : ((struct dm_struct *)0);
}

u8 rtw_rust_hal_dm_phydm_iqk_in_progress_flag(struct dm_struct *phydm)
{
	if (!phydm)
		return _FALSE;
	return phydm->rf_calibrate_info.is_iqk_in_progress;
}

void rtw_rust_hal_dm_err_iqk_in_progress(void)
{
	host_hal_dm_phydm_igi_iqk_trace_err();
}
