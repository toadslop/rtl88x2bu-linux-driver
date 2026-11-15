// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: USB PS + xmit/mgnt dispatch (W4-87). */

typedef int s32;
typedef unsigned char u8;
typedef struct _adapter _adapter;
struct xmit_frame;

u8 rtw_hal_intf_ps_func(_adapter *p, int id, u8 *v) { (void)p; (void)id; (void)v; return 0; }
#ifdef CONFIG_RTW_MGMT_QUEUE
s32 rtw_hal_mgmt_xmitframe_enqueue(_adapter *p, struct xmit_frame *f)
{
	(void)p; (void)f; return 0;
}
#endif
s32 rtw_hal_xmitframe_enqueue(_adapter *p, struct xmit_frame *f) { (void)p; (void)f; return 0; }
s32 rtw_hal_xmit(_adapter *p, struct xmit_frame *f) { (void)p; (void)f; return 0; }
s32 rtw_hal_mgnt_xmit(_adapter *p, struct xmit_frame *f) { (void)p; (void)f; return 0; }