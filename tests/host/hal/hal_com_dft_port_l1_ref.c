// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-131 (body in rust/hal_com_dft_port.rs). */

typedef unsigned char u8;
typedef int s32;

s32 rtw_hal_set_default_port_id_cmd(void *adapter, u8 mac_id)
{
	(void)adapter;
	(void)mac_id;
	return 1;
}

s32 rtw_set_default_port_id(void *adapter)
{
	(void)adapter;
	return 1;
}