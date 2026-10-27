// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-178 — mirrors core/rtw_recv.c static tables. */

#include "host_recv_signal.h"

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
unsigned char host_signal_stat_calc_profile[HOST_SIGNAL_STAT_CALC_PROFILE_MAX][3] = {
	{ 4, 1, 0 },
	{ 3, 7, 0 },
	{ 0, 10, 0 },
};
#endif

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
