// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_rcr_types.h"

void rtw_hal_get_hwreg(struct _adapter *padapter, u32 variable, u8 *val);
u8 rtw_hal_set_hwreg(struct _adapter *padapter, u32 variable, u8 *val);

/* C oracle mirrored from hal/hal_com.c (W4-127 leaf only). */

#ifdef RUST_HAL_COM_RCR_ORACLE
#define RCR_CHECK rtw_hal_rcr_check_c_oracle
#define RCR_ADD rtw_hal_rcr_add_c_oracle
#define RCR_CLEAR rtw_hal_rcr_clear_c_oracle
#else
#define RCR_CHECK rtw_hal_rcr_check
#define RCR_ADD rtw_hal_rcr_add
#define RCR_CLEAR rtw_hal_rcr_clear
#endif

u8 RCR_CHECK(struct _adapter *adapter, u32 check_bit)
{
	u32 rcr = host_hal_rcr_receive_config(adapter);

	if ((rcr & check_bit) == check_bit)
		return 1;
	return 0;
}

u8 RCR_ADD(struct _adapter *adapter, u32 add)
{
	u32 rcr;
	u8 ret = _SUCCESS;

	rtw_hal_get_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr);
	rcr |= add;
	if (rcr != host_hal_rcr_receive_config(adapter))
		ret = rtw_hal_set_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr);
	return ret;
}

u8 RCR_CLEAR(struct _adapter *adapter, u32 clear)
{
	u32 rcr;
	u8 ret = _SUCCESS;

	rtw_hal_get_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr);
	rcr &= ~clear;
	if (rcr != host_hal_rcr_receive_config(adapter))
		ret = rtw_hal_set_hwreg(adapter, HW_VAR_RCR, (u8 *)&rcr);
	return ret;
}