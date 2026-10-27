/* SPDX-License-Identifier: GPL-2.0 */
/* Host oracle types for W3-178 recv signal-stat profile + SNAP headers. */

#ifndef HOST_RECV_SIGNAL_H
#define HOST_RECV_SIGNAL_H

#include <stddef.h>

#define HOST_SIGNAL_STAT_CALC_PROFILE_MAX 3
#define HOST_SNAP_ETH_TYPE_LEN 2
#define HOST_SNAP_HEADER_LEN 6

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
extern unsigned char host_signal_stat_calc_profile[HOST_SIGNAL_STAT_CALC_PROFILE_MAX][3];
#endif

extern unsigned char host_rtw_bridge_tunnel_header[HOST_SNAP_HEADER_LEN];
extern unsigned char host_rtw_rfc1042_header[HOST_SNAP_HEADER_LEN];
extern unsigned char host_snap_eth_type_ipx[HOST_SNAP_ETH_TYPE_LEN];
extern unsigned char host_snap_eth_type_appletalk_aarp[HOST_SNAP_ETH_TYPE_LEN];
#ifdef CONFIG_TDLS
extern unsigned char host_snap_eth_type_tdls[HOST_SNAP_ETH_TYPE_LEN];
#endif

#endif /* HOST_RECV_SIGNAL_H */
