// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-134 MSR helpers (body in rust/hal_com_msr.rs). */

struct _adapter;

void rtw_hal_get_msr(struct _adapter *adapter, unsigned char *net_type)
{
	(void)adapter;
	(void)net_type;
}

void rtw_hal_set_msr(struct _adapter *adapter, unsigned char net_type)
{
	(void)adapter;
	(void)net_type;
}