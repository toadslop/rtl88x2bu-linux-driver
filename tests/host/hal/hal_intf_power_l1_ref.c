// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: rtw_hal_power_on + rtw_hal_power_off (W4-67). */

typedef struct _adapter _adapter;

unsigned int rtw_hal_power_on(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

void rtw_hal_power_off(_adapter *padapter)
{
	(void)padapter;
}
