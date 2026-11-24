// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-137 hw_var_set_rcr_am — hal/hal_com_rest.c. */

#include "host_hal_com_rcr_am_types.h"

u8 rtw_hal_rcr_add(struct _adapter *adapter, u32 add);
u8 rtw_hal_rcr_clear(struct _adapter *adapter, u32 clear);

static void hw_var_set_rcr_am(struct _adapter *adapter, u8 enable)
{
	u32 rcr = RCR_AM;

	if (enable)
		rtw_hal_rcr_add(adapter, rcr);
	else
		rtw_hal_rcr_clear(adapter, rcr);
}

u8 rtw_hal_set_hwreg(struct _adapter *adapter, u32 variable, u8 *val)
{
	(void)val;

	switch (variable) {
	case HW_VAR_ON_RCR_AM:
		hw_var_set_rcr_am(adapter, 1);
		return _SUCCESS;
	case HW_VAR_OFF_RCR_AM:
		hw_var_set_rcr_am(adapter, 0);
		return _SUCCESS;
	default:
		return _FAIL;
	}
}