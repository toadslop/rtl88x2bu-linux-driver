// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_gpio_wakeup_types.h"

extern struct host_gpio_wl_trace host_gpio_wl_trace;
void rtw_hal_set_hwreg(struct _adapter *padapter, u32 variable, u8 *val);

void rtw_hal_switch_gpio_wl_ctrl_c_oracle(struct _adapter *padapter, u8 index, u8 enable)
{
	struct hal_data *pHalData = GET_HAL_DATA(padapter);

	if (IS_8723D_SERIES(pHalData->version_id) || IS_8192F_SERIES(pHalData->version_id) ||
	    IS_8822B_SERIES(pHalData->version_id) || IS_8821C_SERIES(pHalData->version_id) ||
	    IS_8822C_SERIES(pHalData->version_id))
		rtw_hal_set_hwreg(padapter, HW_SET_GPIO_WL_CTRL, (u8 *)(&enable));

	if (index == 13 || index == 14)
		rtw_hal_set_hwreg(padapter, HW_SET_GPIO_WL_CTRL, (u8 *)(&enable));
}