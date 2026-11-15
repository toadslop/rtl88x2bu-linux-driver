// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: IOL + thread-mode HAL dispatch (W4-78). */

typedef int s32;
typedef unsigned int u32;
typedef struct _adapter _adapter;
typedef _adapter ADAPTER;
struct xmit_frame;

int rtw_hal_iol_cmd(ADAPTER *adapter, struct xmit_frame *xmit_frame, u32 max_waiting_ms,
		    u32 bndy_cnt)
{
	(void)adapter;
	(void)xmit_frame;
	(void)max_waiting_ms;
	(void)bndy_cnt;
	return 0;
}

s32 rtw_hal_xmit_thread_handler(_adapter *padapter)
{
	(void)padapter;
	return 0;
}

s32 rtw_hal_recv_hdl(_adapter *adapter)
{
	(void)adapter;
	return 0;
}