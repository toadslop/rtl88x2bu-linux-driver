/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#ifndef _RTW_RECV_SIGNAL_H_
#define _RTW_RECV_SIGNAL_H_

#include <drv_types.h>

#ifdef CONFIG_NEW_SIGNAL_STAT_PROCESS
enum recv_signal_stat_calc_profile {
	SIGNAL_STAT_CALC_PROFILE_0 = 0,
	SIGNAL_STAT_CALC_PROFILE_1,
	SIGNAL_STAT_CALC_PROFILE_2,
	SIGNAL_STAT_CALC_PROFILE_MAX
};

extern u8 signal_stat_calc_profile[SIGNAL_STAT_CALC_PROFILE_MAX][3];
#endif /* CONFIG_NEW_SIGNAL_STAT_PROCESS */

extern u8 rtw_bridge_tunnel_header[];
extern u8 rtw_rfc1042_header[];
extern u8 SNAP_ETH_TYPE_IPX[2];
extern u8 SNAP_ETH_TYPE_APPLETALK_AARP[2];
#ifdef CONFIG_TDLS
extern u8 SNAP_ETH_TYPE_TDLS[2];
#endif

#endif /* _RTW_RECV_SIGNAL_H_ */
