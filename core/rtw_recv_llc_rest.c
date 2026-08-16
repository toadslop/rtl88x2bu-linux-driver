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
#define _RTW_RECV_LLC_REST_C_

#ifdef HOST_RECV_LLC_TEST
#include "host_recv_types.h"
#else
#include <drv_types.h>
#endif

#ifdef HOST_RECV_LLC_TEST
typedef unsigned int uint;
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RECV_LLC_TEST)

#ifdef HOST_RECV_LLC_TEST
u8 rtw_bridge_tunnel_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };
u8 rtw_rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };

#define _rtw_memcmp host_rtw_memcmp
#endif

enum rtw_rx_llc_hdl rtw_recv_llc_parse(u8 *msdu, u16 msdu_len)
{
	u16 eth_type;

	if (msdu_len < 8)
		return RTW_RX_LLC_KEEP;

	eth_type = RTW_GET_BE16(msdu + SNAP_SIZE);

	if ((_rtw_memcmp(msdu, rtw_rfc1042_header, SNAP_SIZE)
			&& eth_type != ETH_P_AARP && eth_type != ETH_P_IPX)
		|| _rtw_memcmp(msdu, rtw_bridge_tunnel_header, SNAP_SIZE)) {
		return RTW_RX_LLC_REMOVE;
	}
	return RTW_RX_LLC_KEEP;
}

sint wlanhdr_to_ethhdr(union recv_frame *precvframe, enum rtw_rx_llc_hdl llc_hdl)
{
	u8 *ptr = get_recvframe_data(precvframe);
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;
	sint rmv_len;
	u16 len;
	sint ret = _SUCCESS;

	if (pattrib->encrypt)
		recvframe_pull_tail(precvframe, pattrib->icv_len);

	rmv_len = pattrib->hdrlen + pattrib->iv_len + RATTRIB_GET_MCTRL_LEN(pattrib)
		  + (llc_hdl ? SNAP_SIZE : 0);
	len = precvframe->u.hdr.len - rmv_len;

	ptr = recvframe_pull(precvframe,
			     (rmv_len - sizeof(struct ethhdr) + (llc_hdl ? 2 : 0)));
	if (!ptr) {
		ret = _FAIL;
		goto exiting;
	}

	_rtw_memcpy(ptr, pattrib->dst, ETH_ALEN);
	_rtw_memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);

	if (!llc_hdl) {
		len = htons(len);
		_rtw_memcpy(ptr + 12, &len, 2);
	}

	rtw_rframe_set_os_pkt(precvframe);

exiting:
	return ret;
}

u8 adapter_allow_bmc_data_rx(_adapter *adapter)
{
	if (check_fwstate(&adapter->mlmepriv, WIFI_MONITOR_STATE | WIFI_MP_STATE) == _TRUE)
		return 1;

#ifdef RTW_SIMPLE_CONFIG
	if (MLME_IS_AP(adapter) && adapter->rtw_simple_config)
		return 1;
#endif

	if (MLME_IS_AP(adapter))
		return 0;

	if (rtw_linked_check(adapter) == _FALSE)
		return 0;

	return 1;
}

#endif /* !CONFIG_RUST || HOST_RECV_LLC_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_RECV_LLC_TEST)

struct rx_pkt_attrib *rtw_rust_recv_frame_attrib(union recv_frame *rframe)
{
	return &rframe->u.hdr.attrib;
}

void rtw_rust_recv_frame_pull_tail(union recv_frame *rframe, sint sz)
{
	recvframe_pull_tail(rframe, sz);
}

u8 *rtw_rust_recv_frame_pull(union recv_frame *rframe, sint sz)
{
	return recvframe_pull(rframe, sz);
}

void rtw_rust_rframe_set_os_pkt(union recv_frame *rframe)
{
	rtw_rframe_set_os_pkt(rframe);
}

sint rtw_rust_adapter_fw_state(_adapter *adapter)
{
	return adapter->mlmepriv.fw_state;
}

u8 rtw_rust_adapter_linked(_adapter *adapter)
{
	return rtw_linked_check(adapter) == _TRUE ? 1 : 0;
}

u8 rtw_rust_adapter_simple_config(_adapter *adapter)
{
#ifdef RTW_SIMPLE_CONFIG
	return adapter->rtw_simple_config ? 1 : 0;
#else
	(void)adapter;
	return 0;
#endif
}

u8 rtw_rust_attrib_mesh_ctrl_len(struct rx_pkt_attrib *a)
{
#ifdef CONFIG_RTW_MESH
	return a->mesh_ctrl_len;
#else
	(void)a;
	return 0;
#endif
}

#endif /* CONFIG_RUST && !HOST_RECV_LLC_TEST */
