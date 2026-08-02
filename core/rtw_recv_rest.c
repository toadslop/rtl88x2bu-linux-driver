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
#define _RTW_RECV_REST_C_

#ifdef HOST_RECV_TEST
#include "host_recv_types.h"
#else
#include <drv_types.h>
#endif

#ifdef HOST_RECV_TEST
typedef unsigned int uint;
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RECV_TEST)

#if !defined(HOST_RECV_TEST) || defined(HOST_RECV_WFD_TEST)
bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset)
{
#define DBG_RFRAME_DEL_WFD_IE 0
	u8 *ies = rframe->u.hdr.rx_data + sizeof(struct rtw_ieee80211_hdr_3addr) + ies_offset;
	uint ies_len_ori = rframe->u.hdr.len - (ies - rframe->u.hdr.rx_data);
	uint ies_len;

	ies_len = rtw_del_wfd_ie(ies, ies_len_ori, DBG_RFRAME_DEL_WFD_IE ? __func__ : NULL);
	rframe->u.hdr.len -= ies_len_ori - ies_len;

	return ies_len_ori != ies_len;
}
#endif /* !HOST_RECV_TEST || HOST_RECV_WFD_TEST */

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{
	int ret = _FALSE;
	int value = ATOMIC_INC_RETURN(&sta->continual_no_rx_packet[tid_index]);

	if (value >= MAX_CONTINUAL_NORXPACKET_COUNT)
		ret = _TRUE;

	return ret;
}

void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index)
{
	ATOMIC_SET(&sta->continual_no_rx_packet[tid_index], 0);
}

#endif /* !CONFIG_RUST || HOST_RECV_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_RECV_TEST)

ATOMIC_T *rtw_rust_recv_continual_no_rx(struct sta_info *sta, int tid_index)
{
	return &sta->continual_no_rx_packet[tid_index];
}

u8 *rtw_rust_recv_frame_rx_data(union recv_frame *rframe)
{
	return rframe->u.hdr.rx_data;
}

uint rtw_rust_recv_frame_len(union recv_frame *rframe)
{
	return rframe->u.hdr.len;
}

void rtw_rust_recv_frame_set_len(union recv_frame *rframe, uint len)
{
	rframe->u.hdr.len = len;
}

uint rtw_rust_del_wfd_ie(u8 *ies, uint ies_len_ori, const char *msg)
{
	return rtw_del_wfd_ie(ies, ies_len_ori, msg);
}

#endif /* CONFIG_RUST && !HOST_RECV_TEST */