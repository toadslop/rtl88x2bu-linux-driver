/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#define _RTW_AP_STA_ALIVE_C_

#ifdef HOST_AP_STA_ALIVE_TEST
#include "host_ap_sta_alive_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

u8 chk_sta_is_alive(struct sta_info *psta)
{
	u8 ret = _FALSE;
#ifdef DBG_EXPIRATION_CHK
	RTW_INFO("sta:"MAC_FMT", rssi:%d, rx:"STA_PKTS_FMT", expire_to:%u, %s%ssq_len:%u\n"
		 , MAC_ARG(psta->cmn.mac_addr)
		 , psta->cmn.rssi_stat.rssi
		 , STA_RX_PKTS_DIFF_ARG(psta)
		 , psta->expire_to
		 , psta->state & WIFI_SLEEP_STATE ? "PS, " : ""
		 , psta->state & WIFI_STA_ALIVE_CHK_STATE ? "SAC, " : ""
		 , psta->sleepq_len
		);
#endif

	if ((psta->sta_stats.last_rx_data_pkts + psta->sta_stats.last_rx_ctrl_pkts) ==
	    (psta->sta_stats.rx_data_pkts + psta->sta_stats.rx_ctrl_pkts)) {
#if 0
		if (psta->state & WIFI_SLEEP_STATE)
			ret = _TRUE;
#endif
	} else
		ret = _TRUE;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(psta->padapter)) {
		u8 bcn_alive, hwmp_alive;

		hwmp_alive = (psta->sta_stats.rx_hwmp_pkts !=
			      psta->sta_stats.last_rx_hwmp_pkts);
		bcn_alive = (psta->sta_stats.rx_beacon_pkts !=
			     psta->sta_stats.last_rx_beacon_pkts);
		psta->alive = ret || hwmp_alive || bcn_alive;
		ret = ret || hwmp_alive;
	}
#endif

	sta_update_last_rx_pkts(psta);

	return ret;
}
