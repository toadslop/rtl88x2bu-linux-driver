// SPDX-License-Identifier: GPL-2.0
#include "host_ap_rf18_restore_types.h"

static u32 host_reg[2];
static u8 host_union_ok;
static u8 host_union_ch, host_union_bw, host_union_offset;
static u8 host_set_channel_called;
static u8 host_last_ch, host_last_offset, host_last_bw;

void host_rf18_reset(void)
{
	host_reg[0] = host_reg[1] = 0x100;
	host_union_ok = 0;
	host_set_channel_called = 0;
}

void host_rf18_set_reg(u32 path, u32 val)
{
	if (path < 2)
		host_reg[path] = val;
}

void host_rf18_set_union_ok(u8 ok, u8 ch, u8 bw, u8 offset)
{
	host_union_ok = ok;
	host_union_ch = ch;
	host_union_bw = bw;
	host_union_offset = offset;
}

u8 host_rf18_set_channel_called(void)
{
	return host_set_channel_called;
}

void host_rf18_last_set_channel(u8 *ch, u8 *offset, u8 *bw)
{
	*ch = host_last_ch;
	*offset = host_last_offset;
	*bw = host_last_bw;
}

u8 host_rf18_hal_current_channel(void)
{
	return 0;
}

u32 rtw_hal_read_rfreg(_adapter *padapter, u32 path, u32 addr, u32 mask)
{
	(void)padapter;
	(void)addr;
	(void)mask;
	if (path >= 2)
		return 0;
	return host_reg[path];
}

u8 rtw_mi_get_ch_setting_union(_adapter *padapter, u8 *ch, u8 *bw, u8 *offset)
{
	(void)padapter;
	if (!host_union_ok)
		return _FALSE;
	*ch = host_union_ch;
	*bw = host_union_bw;
	*offset = host_union_offset;
	return _TRUE;
}

void set_channel_bwmode(_adapter *padapter, u8 ch, u8 offset, u8 bw)
{
	(void)padapter;
	host_set_channel_called = 1;
	host_last_ch = ch;
	host_last_offset = offset;
	host_last_bw = bw;
	padapter->hal_data.current_channel = 0;
}
