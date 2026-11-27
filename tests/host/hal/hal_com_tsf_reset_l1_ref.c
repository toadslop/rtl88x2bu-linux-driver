// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-143 (body in rust/hal_com_tsf_reset.rs). */

typedef unsigned char u8;
typedef int s32;

s32 rtw_hal_reset_tsf(void *adapter, u8 reset_port)
{
	(void)adapter;
	(void)reset_port;
	return 1;
}