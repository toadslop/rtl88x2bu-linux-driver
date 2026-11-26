// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: GPIO pin MMIO exports (W4-140). */

struct _adapter;

typedef unsigned char u8;

void rtw_hal_set_output_gpio(struct _adapter *padapter, u8 index, u8 outputval)
{
	(void)padapter;
	(void)index;
	(void)outputval;
}

void rtw_hal_set_input_gpio(struct _adapter *padapter, u8 index)
{
	(void)padapter;
	(void)index;
}