// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-51 reqtxrpt + MI MAC addr (body in rust/hal_com.rs). */

struct _adapter;

void rtw_hal_reqtxrpt(struct _adapter *padapter, unsigned char macid)
{
	(void)padapter;
	(void)macid;
}

void rtw_mi_set_mac_addr(struct _adapter *adapter)
{
	(void)adapter;
}
