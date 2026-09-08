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
#define _RTW_AP_STA_IE_RATES_C_

#ifdef HOST_AP_STA_IE_TEST
#include "host_ap_sta_ie_rates_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_STA_IE) || defined(HOST_AP_STA_IE_TEST)

u16 rtw_ap_parse_sta_supported_rates(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len)
{
	u8 rate_set[12];
	u8 rate_num;
	int i;
	u16 status = _STATS_SUCCESSFUL_;

	rtw_ies_get_supported_rate(tlv_ies, tlv_ies_len, rate_set, &rate_num);
	if (rate_num == 0) {
		RTW_INFO(FUNC_ADPT_FMT" sta "MAC_FMT" with no supported rate\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(sta->cmn.mac_addr));
		status = _STATS_FAILURE_;
		goto exit;
	}

	_rtw_memcpy(sta->bssrateset, rate_set, rate_num);
	sta->bssratelen = rate_num;

	if (MLME_IS_AP(adapter)) {
		/* this function force only CCK rates to be bassic rate... */
		UpdateBrateTblForSoftAP(sta->bssrateset, sta->bssratelen);
	}

	sta->flags |= WLAN_STA_NONERP;
	for (i = 0; i < (int)sta->bssratelen; i++) {
		if ((sta->bssrateset[i] & 0x7f) > 22) {
			sta->flags &= ~WLAN_STA_NONERP;
			break;
		}
	}

exit:
	return status;
}

#endif /* !CONFIG_RUST_AP_STA_IE || HOST_AP_STA_IE_TEST */
