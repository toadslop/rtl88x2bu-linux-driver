// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-123 GPIO WL ctrl (body in rust/hal_com_gpio_wakeup.rs). */

struct _adapter;

void rtw_hal_switch_gpio_wl_ctrl(struct _adapter *padapter, unsigned char index,
				 unsigned char enable)
{
	(void)padapter;
	(void)index;
	(void)enable;
}