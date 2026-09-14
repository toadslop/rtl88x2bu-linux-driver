/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_STA_RA_C_

#ifdef HOST_AP_STA_RA_TEST
#include "host_ap_sta_ra_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST_AP_STA_RA) || defined(HOST_AP_STA_RA_TEST)

void rtw_ap_update_sta_ra_info(_adapter *padapter, struct sta_info *psta)
{
	unsigned char sta_band = 0;
	u64 tx_ra_bitmap = 0;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	WLAN_BSSID_EX *pcur_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;

	if (!psta)
		return;

	if (!(psta->state & WIFI_ASOC_STATE))
		return;

	rtw_hal_update_sta_ra_info(padapter, psta);
	tx_ra_bitmap = psta->cmn.ra_info.ramask;

	if (pcur_network->Configuration.DSConfig > 14) {

		if (tx_ra_bitmap & 0xffff000)
			sta_band |= WIRELESS_11_5N;

		if (tx_ra_bitmap & 0xff0)
			sta_band |= WIRELESS_11A;

		/* 5G band */
#ifdef CONFIG_80211AC_VHT
		if (psta->vhtpriv.vht_option)
			sta_band = WIRELESS_11_5AC;
#endif
	} else {
		if (tx_ra_bitmap & 0xffff000)
			sta_band |= WIRELESS_11_24N;

		if (tx_ra_bitmap & 0xff0)
			sta_band |= WIRELESS_11G;

		if (tx_ra_bitmap & 0x0f)
			sta_band |= WIRELESS_11B;
	}

	psta->wireless_mode = sta_band;
	rtw_hal_update_sta_wset(padapter, psta);
#ifndef HOST_AP_STA_RA_TEST
	RTW_INFO("%s=> mac_id:%d , tx_ra_bitmap:0x%016llx, networkType:0x%02x\n",
		 __FUNCTION__, psta->cmn.mac_id, tx_ra_bitmap, psta->wireless_mode);
#endif
}

#endif /* !CONFIG_RUST_AP_STA_RA || HOST_AP_STA_RA_TEST */
