// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stub for W4-135 HALMAC identity helpers (body in rust/hal_com_halmac_identity.rs). */

struct _adapter;

void rtw_hal_set_bssid(struct _adapter *adapter, unsigned char *val)
{
	(void)adapter;
	(void)val;
}