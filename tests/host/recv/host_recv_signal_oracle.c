// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-178 — mirrors core/rtw_recv.c static tables. */

#ifdef HOST_RECV_SIGNAL_KERNEL_NAMES
typedef unsigned char u8;
#define CONFIG_NEW_SIGNAL_STAT_PROCESS 1
#define CONFIG_TDLS 1
#define SIGNAL_STAT_CALC_PROFILE_MAX 3
#else
#include "host_recv_signal.h"
#endif

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
#ifdef HOST_RECV_SIGNAL_KERNEL_NAMES
u8 signal_stat_calc_profile[SIGNAL_STAT_CALC_PROFILE_MAX][3] = {
#else
unsigned char host_signal_stat_calc_profile[HOST_SIGNAL_STAT_CALC_PROFILE_MAX][3] = {
#endif
	{ 4, 1, 0 },
	{ 3, 7, 0 },
	{ 0, 10, 0 },
};
#endif

#ifdef HOST_RECV_SIGNAL_KERNEL_NAMES
u8 rtw_bridge_tunnel_header[6] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };
u8 rtw_rfc1042_header[6] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };
u8 SNAP_ETH_TYPE_IPX[2] = { 0x81, 0x37 };
u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = { 0x80, 0xf3 };
#ifdef CONFIG_TDLS
u8 SNAP_ETH_TYPE_TDLS[2] = { 0x89, 0x0d };
#endif
#else
unsigned char host_rtw_bridge_tunnel_header[HOST_SNAP_HEADER_LEN] = {
	0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8,
};
unsigned char host_rtw_rfc1042_header[HOST_SNAP_HEADER_LEN] = {
	0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00,
};
unsigned char host_snap_eth_type_ipx[HOST_SNAP_ETH_TYPE_LEN] = { 0x81, 0x37 };
unsigned char host_snap_eth_type_appletalk_aarp[HOST_SNAP_ETH_TYPE_LEN] = { 0x80, 0xf3 };
#ifdef CONFIG_TDLS
unsigned char host_snap_eth_type_tdls[HOST_SNAP_ETH_TYPE_LEN] = { 0x89, 0x0d };
#endif
#endif
