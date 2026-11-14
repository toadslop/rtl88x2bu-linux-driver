// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: DBG_CONFIG_ERROR_DETECT sreset dispatch (W4-74). */

typedef unsigned char u8;
typedef struct _adapter _adapter;
typedef _Bool bool;

void rtw_hal_sreset_init(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_sreset_reset(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_sreset_reset_value(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_sreset_xmit_status_check(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_sreset_linked_status_check(_adapter *padapter)
{
	(void)padapter;
}

u8 rtw_hal_sreset_get_wifi_status(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

bool rtw_hal_sreset_inprogress(_adapter *padapter)
{
	(void)padapter;
	return 0;
}