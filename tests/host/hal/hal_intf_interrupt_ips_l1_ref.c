// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: interrupt + IPS HAL wrappers (W4-70). */

typedef unsigned char u8;
typedef struct _adapter _adapter;

void rtw_hal_enable_interrupt(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_disable_interrupt(_adapter *padapter)
{
	(void)padapter;
}

u8 rtw_hal_check_ips_status(_adapter *padapter)
{
	(void)padapter;
	return 0;
}