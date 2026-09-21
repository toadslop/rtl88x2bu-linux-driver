// SPDX-License-Identifier: GPL-2.0
#include "host_tdls_ht_types.h"

void rtw_tdls_process_ht_cap(_adapter *padapter, struct sta_info *ptdls_sta, u8 *data,
			     u8 length)
{
	struct mlme_ext_info *pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;
	struct ht_priv *phtpriv = &padapter->mlmepriv.htpriv;
	u8 max_ampdu_len, min_mpdu_spacing;

	_rtw_memset(ptdls_sta->htpriv.ht_cap, 0, HT_CAP_LEN);
	if (data && length >= HT_CAP_LEN) {
		ptdls_sta->flags |= WLAN_STA_HT | WLAN_STA_WME;
		_rtw_memcpy(ptdls_sta->htpriv.ht_cap, data, HT_CAP_LEN);
	} else {
		ptdls_sta->flags &= ~WLAN_STA_HT;
		return;
	}

	if (padapter->registrypriv.ht_enable == _TRUE &&
	    is_supported_ht(padapter->registrypriv.wireless_mode)) {
		ptdls_sta->htpriv.ht_option = _TRUE;
		ptdls_sta->qos_option = _TRUE;
	} else {
		ptdls_sta->htpriv.ht_option = _FALSE;
		ptdls_sta->qos_option = _FALSE;
		return;
	}

	if (padapter->registrypriv.ampu_enable == 1)
		ptdls_sta->htpriv.ampdu_enable = _TRUE;

	if ((pmlmeinfo->ap_ampdu_para & 0x3) > (data[2] & 0x3))
		max_ampdu_len = (data[2] & 0x3);
	else
		max_ampdu_len = (pmlmeinfo->ap_ampdu_para & 0x3);
	if ((pmlmeinfo->ap_ampdu_para & 0x1c) > (data[2] & 0x1c))
		min_mpdu_spacing = (pmlmeinfo->ap_ampdu_para & 0x1c);
	else
		min_mpdu_spacing = (data[2] & 0x1c);
	ptdls_sta->htpriv.rx_ampdu_min_spacing = max_ampdu_len | min_mpdu_spacing;

	if (phtpriv->sgi_20m == _TRUE &&
	    (ht_cap_info(ptdls_sta->htpriv.ht_cap) & cpu_to_le16(IEEE80211_HT_CAP_SGI_20)))
		ptdls_sta->htpriv.sgi_20m = _TRUE;

	if (ht_cap_info(ptdls_sta->htpriv.ht_cap) & cpu_to_le16(IEEE80211_HT_CAP_SUP_WIDTH)) {
		if (padapter->mlmeextpriv.cur_bwmode >= CHANNEL_WIDTH_40)
			ptdls_sta->bw_mode = CHANNEL_WIDTH_40;
		ptdls_sta->htpriv.ch_offset = padapter->mlmeextpriv.cur_ch_offset;
	}
}
