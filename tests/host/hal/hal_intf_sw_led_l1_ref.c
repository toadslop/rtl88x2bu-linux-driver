// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_sw_led_init + rtw_hal_sw_led_deinit (W4-68). */

typedef struct _adapter _adapter;

#ifdef CONFIG_RTW_SW_LED
void rtw_hal_sw_led_init(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_sw_led_deinit(_adapter *padapter)
{
	(void)padapter;
}
#endif
