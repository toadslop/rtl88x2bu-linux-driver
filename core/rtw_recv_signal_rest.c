/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_RECV_SIGNAL_REST_C_

#include <drv_types.h>
#include <rtw_recv_signal.h>

#if !defined(CONFIG_RUST_RECV_SIGNAL)

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
u8 signal_stat_calc_profile[SIGNAL_STAT_CALC_PROFILE_MAX][3] = {
	{4, 1, 0},
	{3, 7, 0},
	{0, 10, 0},
};
#endif /* CONFIG_NEW_SIGNAL_STAT_PROCESS */

u8 rtw_bridge_tunnel_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };
u8 rtw_rfc1042_header[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };
u8 SNAP_ETH_TYPE_IPX[2] = {0x81, 0x37};
u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = {0x80, 0xf3};
#ifdef CONFIG_TDLS
u8 SNAP_ETH_TYPE_TDLS[2] = {0x89, 0x0d};
#endif

#endif /* !CONFIG_RUST_RECV_SIGNAL */
