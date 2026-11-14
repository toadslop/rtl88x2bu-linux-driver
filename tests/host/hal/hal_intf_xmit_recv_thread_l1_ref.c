// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: xmit/recv priv + SDIO thread dispatch (W4-83). */

typedef int s32;
typedef struct _adapter _adapter;

s32 rtw_hal_init_xmit_priv(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

void rtw_hal_free_xmit_priv(_adapter *padapter)
{
	(void)padapter;
}

s32 rtw_hal_init_recv_priv(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

void rtw_hal_free_recv_priv(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_start_thread(_adapter *padapter)
{
	(void)padapter;
}

void rtw_hal_stop_thread(_adapter *padapter)
{
	(void)padapter;
}