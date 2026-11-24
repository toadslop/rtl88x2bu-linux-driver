// SPDX-License-Identifier: GPL-2.0
/* Rust rtw_hal_set_hwreg path for W4-137 L2. */

#include "host_hal_com_rcr_am_types.h"

void rtw_rust_hw_var_set_rcr_am(struct _adapter *adapter, u8 enable);

u8 rtw_hal_set_hwreg(struct _adapter *adapter, u32 variable, u8 *val)
{
	(void)val;

	switch (variable) {
	case HW_VAR_ON_RCR_AM:
		rtw_rust_hw_var_set_rcr_am(adapter, 1);
		return _SUCCESS;
	case HW_VAR_OFF_RCR_AM:
		rtw_rust_hw_var_set_rcr_am(adapter, 0);
		return _SUCCESS;
	default:
		return _FAIL;
	}
}