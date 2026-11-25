// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-129 TSF iface ctrl (bodies in rust/hal_com_tsf.rs). */

struct _adapter;

void rtw_iface_enable_tsf_update(struct _adapter *adapter)
{
	(void)adapter;
}

void rtw_iface_disable_tsf_update(struct _adapter *adapter)
{
	(void)adapter;
}

void rtw_hal_periodic_tsf_update_chk(struct _adapter *adapter)
{
	(void)adapter;
}

void rtw_hal_periodic_tsf_update_end_timer_hdl(void *ctx)
{
	(void)ctx;
}