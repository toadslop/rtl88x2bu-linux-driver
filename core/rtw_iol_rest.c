/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_IOL_REST_C_

#ifdef HOST_IOL_TEST
#include "host_iol_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_IOL_TEST)

int rtw_IOL_append_cmds(struct xmit_frame *xmit_frame, u8 *IOL_cmds, u32 cmd_len)
{
	struct pkt_attrib *pattrib = &xmit_frame->attrib;
	u16 buf_offset;
	u32 ori_len;

	buf_offset = TXDESC_OFFSET;
	ori_len = buf_offset + pattrib->pktlen;

	if (ori_len + cmd_len + 8 > MAX_XMITBUF_SZ) {
		RTW_INFO("%s %u is large than MAX_XMITBUF_SZ:%u, can't accommodate new cmds\n",
			 __func__, ori_len + cmd_len + 8, MAX_XMITBUF_SZ);
		return _FAIL;
	}

	_rtw_memcpy(xmit_frame->buf_addr + buf_offset + pattrib->pktlen, IOL_cmds, cmd_len);
	pattrib->pktlen += cmd_len;
	pattrib->last_txcmdsz += cmd_len;

	return _SUCCESS;
}

int rtw_IOL_append_LLT_cmd(struct xmit_frame *xmit_frame, u8 page_boundary)
{
	IOL_CMD cmd = {0x0, IOL_CMD_LLT, 0x0, 0x0};

	RTW_PUT_BE32((u8 *)&cmd.value, (u32)page_boundary);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int _rtw_IOL_append_WB_cmd(struct xmit_frame *xmit_frame, u16 addr, u8 value)
{
	IOL_CMD cmd = {0x0, IOL_CMD_WB_REG, 0x0, 0x0};

	RTW_PUT_BE16((u8 *)&cmd.address, (u16)addr);
	RTW_PUT_BE32((u8 *)&cmd.value, (u32)value);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int _rtw_IOL_append_WW_cmd(struct xmit_frame *xmit_frame, u16 addr, u16 value)
{
	IOL_CMD cmd = {0x0, IOL_CMD_WW_REG, 0x0, 0x0};

	RTW_PUT_BE16((u8 *)&cmd.address, (u16)addr);
	RTW_PUT_BE32((u8 *)&cmd.value, (u32)value);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int _rtw_IOL_append_WD_cmd(struct xmit_frame *xmit_frame, u16 addr, u32 value)
{
	IOL_CMD cmd = {0x0, IOL_CMD_WD_REG, 0x0, 0x0};

	RTW_PUT_BE16((u8 *)&cmd.address, (u16)addr);
	RTW_PUT_BE32((u8 *)&cmd.value, (u32)value);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int rtw_IOL_append_DELAY_US_cmd(struct xmit_frame *xmit_frame, u16 us)
{
	IOL_CMD cmd = {0x0, IOL_CMD_DELAY_US, 0x0, 0x0};

	RTW_PUT_BE32((u8 *)&cmd.value, (u32)us);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int rtw_IOL_append_DELAY_MS_cmd(struct xmit_frame *xmit_frame, u16 ms)
{
	IOL_CMD cmd = {0x0, IOL_CMD_DELAY_MS, 0x0, 0x0};

	RTW_PUT_BE32((u8 *)&cmd.value, (u32)ms);

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&cmd, 8);
}

int rtw_IOL_append_END_cmd(struct xmit_frame *xmit_frame)
{
	IOL_CMD end_cmd = {0x0, IOL_CMD_END, 0x0, 0x0};

	return rtw_IOL_append_cmds(xmit_frame, (u8 *)&end_cmd, 8);
}

#endif /* !CONFIG_RUST || HOST_IOL_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_IOL_TEST)

u32 rtw_rust_iol_attrib_pktlen(struct xmit_frame *xframe)
{
	return xframe->attrib.pktlen;
}

u32 rtw_rust_iol_attrib_last_txcmdsz(struct xmit_frame *xframe)
{
	return xframe->attrib.last_txcmdsz;
}

void rtw_rust_iol_set_attrib_lengths(struct xmit_frame *xframe, u32 pktlen, u32 last_txcmdsz)
{
	xframe->attrib.pktlen = pktlen;
	xframe->attrib.last_txcmdsz = last_txcmdsz;
}

u8 *rtw_rust_iol_xframe_buf_addr(struct xmit_frame *xframe)
{
	return xframe->buf_addr;
}

u16 rtw_rust_iol_txdesc_offset(void)
{
	return TXDESC_OFFSET;
}

u32 rtw_rust_iol_max_xmitbuf_sz(void)
{
	return MAX_XMITBUF_SZ;
}

void rtw_rust_iol_overflow_log(u32 needed, u32 max_sz)
{
	RTW_INFO("%s %u is large than MAX_XMITBUF_SZ:%u, can't accommodate new cmds\n",
		 __func__, needed, max_sz);
}

#endif /* CONFIG_RUST && !HOST_IOL_TEST */
