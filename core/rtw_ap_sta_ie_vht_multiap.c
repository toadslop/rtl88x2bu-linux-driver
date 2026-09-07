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
#define _RTW_AP_STA_IE_VHT_MULTIAP_C_

#ifdef HOST_AP_STA_IE_TEST
#include "host_ap_sta_ie_vht_multiap_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_STA_IE) || defined(HOST_AP_STA_IE_TEST)

void rtw_ap_parse_sta_vht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;

	sta->flags &= ~WLAN_STA_VHT;

#ifdef CONFIG_80211AC_VHT
	if (mlme->vhtpriv.vht_option == _FALSE)
		goto exit;

	_rtw_memset(&sta->vhtpriv, 0, sizeof(struct vht_priv));
	if (elems->vht_capabilities && elems->vht_capabilities_len == VHT_CAP_IE_LEN) {
		sta->flags |= WLAN_STA_VHT;
		_rtw_memcpy(sta->vhtpriv.vht_cap, elems->vht_capabilities, VHT_CAP_IE_LEN);

		if (elems->vht_operation && elems->vht_operation_len== VHT_OP_IE_LEN) {
			_rtw_memcpy(sta->vhtpriv.vht_op, elems->vht_operation, VHT_OP_IE_LEN);
			sta->vhtpriv.op_present = 1;
		}

		if (elems->vht_op_mode_notify && elems->vht_op_mode_notify_len == 1) {
			_rtw_memcpy(&sta->vhtpriv.vht_op_mode_notify, elems->vht_op_mode_notify, 1);
			sta->vhtpriv.notify_present = 1;
		}
	}
exit:
#endif

	return;
}

void rtw_ap_parse_sta_multi_ap_ie(_adapter *adapter, struct sta_info *sta, u8 *ies, int ies_len)
{
	sta->flags &= ~WLAN_STA_MULTI_AP;

#ifdef CONFIG_RTW_MULTI_AP
	if (adapter->multi_ap
		&& (rtw_get_multi_ap_ie_ext(ies, ies_len) & MULTI_AP_BACKHAUL_STA)
	) {
		if (adapter->multi_ap & MULTI_AP_BACKHAUL_BSS) /* with backhaul bss, enable WDS */
			sta->flags |= WLAN_STA_MULTI_AP | WLAN_STA_WDS;
		else if (adapter->multi_ap & MULTI_AP_FRONTHAUL_BSS) /* fronthaul bss only */
			sta->flags |= WLAN_STA_MULTI_AP;
	}
#endif
}

#endif /* !CONFIG_RUST_AP_STA_IE */
