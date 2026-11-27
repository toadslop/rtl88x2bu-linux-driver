// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-144 (body in rust/hal_com_btc_port.rs). */

typedef int s32;

s32 rtw_hal_set_wifi_btc_port_id_cmd(void *adapter)
{
	(void)adapter;
	return 1;
}