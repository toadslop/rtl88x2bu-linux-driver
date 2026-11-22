// SPDX-License-Identifier: GPL-2.0
/* L1 stubs for W4-127 RCR leaf (body in rust/hal_com_rcr.rs). */

struct _adapter;

unsigned char rtw_hal_rcr_check(struct _adapter *adapter, unsigned int check_bit)
{
	(void)adapter;
	(void)check_bit;
	return 0;
}

unsigned char rtw_hal_rcr_add(struct _adapter *adapter, unsigned int add)
{
	(void)adapter;
	(void)add;
	return 1;
}

unsigned char rtw_hal_rcr_clear(struct _adapter *adapter, unsigned int clear)
{
	(void)adapter;
	(void)clear;
	return 1;
}

void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, unsigned char self_action)
{
	(void)adapter;
	(void)self_action;
}

void rtw_hal_rcr_set_chk_bssid_act_non(struct _adapter *adapter)
{
	(void)adapter;
}