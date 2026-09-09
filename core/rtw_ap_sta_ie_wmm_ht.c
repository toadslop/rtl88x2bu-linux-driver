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
#define _RTW_AP_STA_IE_WMM_HT_C_

#ifdef HOST_AP_STA_IE_TEST
#include "host_ap_sta_ie_wmm_ht_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_STA_IE) || defined(HOST_AP_STA_IE_TEST)

void rtw_ap_parse_sta_wmm_ie(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;
	unsigned char WMM_IE[] = {0x00, 0x50, 0xf2, 0x02, 0x00, 0x01};
	u8 *p;

	sta->flags &= ~WLAN_STA_WME;
	sta->qos_option = 0;
	sta->qos_info = 0;
	sta->has_legacy_ac = _TRUE;
	sta->uapsd_vo = 0;
	sta->uapsd_vi = 0;
	sta->uapsd_be = 0;
	sta->uapsd_bk = 0;

	if (!mlme->qospriv.qos_option)
		goto exit;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		/* QoS is mandatory in mesh */
		sta->flags |= WLAN_STA_WME;
	}
#endif

	p = rtw_get_ie_ex(tlv_ies, tlv_ies_len, WLAN_EID_VENDOR_SPECIFIC, WMM_IE, 6, NULL, NULL);
	if (!p)
		goto exit;

	sta->flags |= WLAN_STA_WME;
	sta->qos_option = 1;
	sta->qos_info = *(p + 8);
	sta->max_sp_len = (sta->qos_info >> 5) & 0x3;

	if ((sta->qos_info & 0xf) != 0xf)
		sta->has_legacy_ac = _TRUE;
	else
		sta->has_legacy_ac = _FALSE;

	if (sta->qos_info & 0xf) {
		if (sta->qos_info & BIT(0))
			sta->uapsd_vo = BIT(0) | BIT(1);
		else
			sta->uapsd_vo = 0;

		if (sta->qos_info & BIT(1))
			sta->uapsd_vi = BIT(0) | BIT(1);
		else
			sta->uapsd_vi = 0;

		if (sta->qos_info & BIT(2))
			sta->uapsd_bk = BIT(0) | BIT(1);
		else
			sta->uapsd_bk = 0;

		if (sta->qos_info & BIT(3))
			sta->uapsd_be = BIT(0) | BIT(1);
		else
			sta->uapsd_be = 0;
	}

exit:
	return;
}

void rtw_ap_parse_sta_ht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;

	sta->flags &= ~WLAN_STA_HT;

#ifdef CONFIG_80211N_HT
	if (mlme->htpriv.ht_option == _FALSE)
		goto exit;

	/* save HT capabilities in the sta object */
	_rtw_memset(&sta->htpriv.ht_cap, 0, sizeof(struct rtw_ieee80211_ht_cap));
	if (elems->ht_capabilities && elems->ht_capabilities_len >= sizeof(struct rtw_ieee80211_ht_cap)) {
		sta->flags |= WLAN_STA_HT;
		sta->flags |= WLAN_STA_WME;
		_rtw_memcpy(&sta->htpriv.ht_cap, elems->ht_capabilities, sizeof(struct rtw_ieee80211_ht_cap));

		if (elems->ht_operation && elems->ht_operation_len == HT_OP_IE_LEN) {
			_rtw_memcpy(sta->htpriv.ht_op, elems->ht_operation, HT_OP_IE_LEN);
			sta->htpriv.op_present = 1;
		}
	}
exit:
#endif

	return;
}

#endif /* !CONFIG_RUST_AP_STA_IE */
