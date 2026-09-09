// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_sta_ie.rs (W3-73 PR8). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_STA_IE) && !defined(HOST_AP_STA_IE_TEST)

u16 rtw_rust_ap_sta_get_capability(struct sta_info *sta)
{
	return sta->capability;
}

void rtw_rust_ap_sta_set_capability(struct sta_info *sta, u16 cap)
{
	sta->capability = cap;
}

int rtw_rust_ap_sta_get_flags(struct sta_info *sta)
{
	return sta->flags;
}

void rtw_rust_ap_sta_set_flags(struct sta_info *sta, int flags)
{
	sta->flags = flags;
}

void rtw_rust_ap_sta_or_flags(struct sta_info *sta, int mask)
{
	sta->flags |= mask;
}

void rtw_rust_ap_sta_andnot_flags(struct sta_info *sta, int mask)
{
	sta->flags &= ~mask;
}

void rtw_rust_ap_sta_set_bssrates(struct sta_info *sta, const u8 *rates, u32 len)
{
	_rtw_memcpy(sta->bssrateset, rates, len);
	sta->bssratelen = len;
}

void rtw_rust_ap_sta_finish_rates(struct sta_info *sta, u8 is_ap)
{
	u32 i;

	if (is_ap)
		UpdateBrateTblForSoftAP(sta->bssrateset, sta->bssratelen);
	sta->flags |= WLAN_STA_NONERP;
	for (i = 0; i < sta->bssratelen; i++) {
		if ((sta->bssrateset[i] & 0x7f) > 22) {
			sta->flags &= ~WLAN_STA_NONERP;
			break;
		}
	}
}

u32 rtw_rust_ap_sta_get_qos_option(struct sta_info *sta)
{
	return sta->qos_option;
}

void rtw_rust_ap_sta_clear_wmm(struct sta_info *sta)
{
	sta->flags &= ~WLAN_STA_WME;
	sta->qos_option = 0;
	sta->qos_info = 0;
	sta->has_legacy_ac = _TRUE;
	sta->uapsd_vo = 0;
	sta->uapsd_vi = 0;
	sta->uapsd_be = 0;
	sta->uapsd_bk = 0;
}

void rtw_rust_ap_sta_apply_wmm(struct sta_info *sta, u8 qos_info)
{
	sta->flags |= WLAN_STA_WME;
	sta->qos_option = 1;
	sta->qos_info = qos_info;
	sta->max_sp_len = (qos_info >> 5) & 0x3;
	sta->has_legacy_ac = ((qos_info & 0xf) != 0xf) ? _TRUE : _FALSE;
	if (qos_info & 0xf) {
		if (qos_info & BIT(0))
			sta->uapsd_vo = BIT(0) | BIT(1);
		else
			sta->uapsd_vo = 0;

		if (qos_info & BIT(1))
			sta->uapsd_vi = BIT(0) | BIT(1);
		else
			sta->uapsd_vi = 0;

		if (qos_info & BIT(2))
			sta->uapsd_bk = BIT(0) | BIT(1);
		else
			sta->uapsd_bk = 0;

		if (qos_info & BIT(3))
			sta->uapsd_be = BIT(0) | BIT(1);
		else
			sta->uapsd_be = 0;
	}
}

u8 rtw_rust_ap_mlme_qos_option(_adapter *adapter)
{
	return adapter->mlmepriv.qospriv.qos_option ? 1 : 0;
}

u8 rtw_rust_ap_mlme_ht_option(_adapter *adapter)
{
#ifdef CONFIG_80211N_HT
	return adapter->mlmepriv.htpriv.ht_option;
#else
	return 0;
#endif
}

u8 rtw_rust_ap_mlme_vht_option(_adapter *adapter)
{
#ifdef CONFIG_80211AC_VHT
	return adapter->mlmepriv.vhtpriv.vht_option;
#else
	return 0;
#endif
}

u8 rtw_rust_ap_mlme_is_ap(_adapter *adapter)
{
	return MLME_IS_AP(adapter) ? 1 : 0;
}

#ifdef CONFIG_RTW_MESH
u8 rtw_rust_ap_mlme_is_mesh(_adapter *adapter)
{
	return MLME_IS_MESH(adapter) ? 1 : 0;
}
#endif

void rtw_rust_ap_sta_clear_ht(struct sta_info *sta)
{
	sta->flags &= ~WLAN_STA_HT;
}

void rtw_rust_ap_sta_apply_ht_from_elems(struct sta_info *sta,
					 struct rtw_ieee802_11_elems *elems)
{
#ifdef CONFIG_80211N_HT
	_rtw_memset(&sta->htpriv.ht_cap, 0, sizeof(sta->htpriv.ht_cap));
	if (elems && elems->ht_capabilities &&
	    elems->ht_capabilities_len >= sizeof(struct rtw_ieee80211_ht_cap)) {
		sta->flags |= WLAN_STA_HT | WLAN_STA_WME;
		_rtw_memcpy(&sta->htpriv.ht_cap, elems->ht_capabilities,
			    sizeof(sta->htpriv.ht_cap));

		if (elems->ht_operation &&
		    elems->ht_operation_len == HT_OP_IE_LEN) {
			_rtw_memcpy(sta->htpriv.ht_op, elems->ht_operation,
				    HT_OP_IE_LEN);
			sta->htpriv.op_present = 1;
		}
	}
#endif
}

void rtw_rust_ap_sta_clear_vht(struct sta_info *sta)
{
	sta->flags &= ~WLAN_STA_VHT;
}

void rtw_rust_ap_sta_apply_vht_from_elems(struct sta_info *sta,
					  struct rtw_ieee802_11_elems *elems)
{
#ifdef CONFIG_80211AC_VHT
	_rtw_memset(&sta->vhtpriv, 0, sizeof(sta->vhtpriv));
	if (elems && elems->vht_capabilities &&
	    elems->vht_capabilities_len == VHT_CAP_IE_LEN) {
		sta->flags |= WLAN_STA_VHT;
		_rtw_memcpy(sta->vhtpriv.vht_cap, elems->vht_capabilities,
			    VHT_CAP_IE_LEN);

		if (elems->vht_operation &&
		    elems->vht_operation_len == VHT_OP_IE_LEN) {
			_rtw_memcpy(sta->vhtpriv.vht_op, elems->vht_operation,
				    VHT_OP_IE_LEN);
			sta->vhtpriv.op_present = 1;
		}

		if (elems->vht_op_mode_notify &&
		    elems->vht_op_mode_notify_len == 1) {
			_rtw_memcpy(&sta->vhtpriv.vht_op_mode_notify,
				    elems->vht_op_mode_notify, 1);
			sta->vhtpriv.notify_present = 1;
		}
	}
#endif
}

u8 rtw_rust_ap_adapter_multi_ap(_adapter *adapter)
{
#ifdef CONFIG_RTW_MULTI_AP
	return adapter->multi_ap;
#else
	return 0;
#endif
}

void rtw_rust_ap_sta_clear_multi_ap(struct sta_info *sta)
{
	sta->flags &= ~WLAN_STA_MULTI_AP;
}

void rtw_rust_ap_sta_apply_multi_ap(struct sta_info *sta, u8 multi_ap, u8 role)
{
#ifdef CONFIG_RTW_MULTI_AP
	if (multi_ap && (role & MULTI_AP_BACKHAUL_STA)) {
		if (multi_ap & MULTI_AP_BACKHAUL_BSS)
			sta->flags |= WLAN_STA_MULTI_AP | WLAN_STA_WDS;
		else if (multi_ap & MULTI_AP_FRONTHAUL_BSS)
			sta->flags |= WLAN_STA_MULTI_AP;
	}
#else
	(void)multi_ap;
	(void)role;
#endif
}

#endif /* CONFIG_RUST_AP_STA_IE && !HOST_AP_STA_IE_TEST */
