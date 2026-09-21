// SPDX-License-Identifier: GPL-2.0
#include "host_tdls_vht_types.h"

#define GET_HAL_TX_NSS(p) host_tdls_hal_tx_nss(p)

extern u8 host_tdls_hal_tx_nss(_adapter *a);

void rtw_tdls_process_vht_cap(_adapter *padapter, struct sta_info *ptdls_sta, u8 *data,
			      u8 length)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	struct vht_priv *pvhtpriv = &padapter->mlmepriv.vhtpriv;
	u8 cur_ldpc_cap = 0, cur_stbc_cap = 0, tx_nss;
	u8 *pcap_mcs;

	_rtw_memset(&ptdls_sta->vhtpriv, 0, sizeof(ptdls_sta->vhtpriv));
	if (data && length == 12) {
		ptdls_sta->flags |= WLAN_STA_VHT;
		_rtw_memcpy(ptdls_sta->vhtpriv.vht_cap, data, 12);
		ptdls_sta->vhtpriv.vht_op_mode_notify = CHANNEL_WIDTH_80;
	} else {
		ptdls_sta->flags &= ~WLAN_STA_VHT;
		return;
	}

	if (REGSTY_IS_11AC_ENABLE(&padapter->registrypriv) &&
	    is_supported_vht(padapter->registrypriv.wireless_mode) &&
	    (!rfctl->country_ent || COUNTRY_CHPLAN_EN_11AC(rfctl->country_ent)))
		ptdls_sta->vhtpriv.vht_option = _TRUE;
	else
		ptdls_sta->vhtpriv.vht_option = _FALSE;

	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_LDPC(data))
		SET_FLAG(cur_ldpc_cap, (LDPC_VHT_ENABLE_TX | LDPC_VHT_CAP_TX));
	ptdls_sta->vhtpriv.ldpc_cap = cur_ldpc_cap;
	ptdls_sta->vhtpriv.sgi_80m =
		(GET_VHT_CAPABILITY_ELE_SHORT_GI80M(data) & pvhtpriv->sgi_80m) ? _TRUE : _FALSE;
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_STBC(data))
		SET_FLAG(cur_stbc_cap, (STBC_VHT_ENABLE_TX | STBC_VHT_CAP_TX));
	ptdls_sta->vhtpriv.stbc_cap = cur_stbc_cap;
	ptdls_sta->vhtpriv.ampdu_len = GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(data);
	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(data);
	tx_nss = GET_HAL_TX_NSS(padapter);
	rtw_vht_nss_to_mcsmap(tx_nss, ptdls_sta->vhtpriv.vht_mcs_map, pcap_mcs);
	ptdls_sta->vhtpriv.vht_highest_rate =
		rtw_get_vht_highest_rate(ptdls_sta->vhtpriv.vht_mcs_map);
	if (ptdls_sta->vhtpriv.vht_option)
		ptdls_sta->ra_is_vht = _TRUE;
}

#define hal_is_bw_support(a, b) host_tdls_hal_bw_support((a), (b))

void rtw_tdls_process_vht_operation(_adapter *padapter, struct sta_info *ptdls_sta,
				    u8 *data, u8 length)
{
	(void)length;
	if (GET_VHT_OPERATION_ELE_CHL_WIDTH(data) >= 1) {
		u8 operation_bw = CHANNEL_WIDTH_80;

		if (hal_is_bw_support(padapter, operation_bw) &&
		    REGSTY_IS_BW_5G_SUPPORT(adapter_to_regsty(padapter), operation_bw) &&
		    operation_bw <= padapter->mlmeextpriv.cur_bwmode)
			ptdls_sta->bw_mode = operation_bw;
		else
			ptdls_sta->bw_mode = padapter->mlmeextpriv.cur_bwmode;
	} else {
		ptdls_sta->bw_mode = padapter->mlmeextpriv.cur_bwmode;
	}
}

void rtw_tdls_process_vht_op_mode_notify(_adapter *padapter, struct sta_info *ptdls_sta,
					 u8 *data, u8 length)
{
	struct vht_priv *pvhtpriv = &padapter->mlmepriv.vhtpriv;
	u8 target_bw, target_rxss, vht_mcs_map[2];

	(void)length;
	if (pvhtpriv->vht_option == _FALSE)
		return;
	target_bw = GET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(data);
	target_rxss = GET_VHT_OPERATING_MODE_FIELD_RX_NSS(data) + 1;
	if (hal_is_bw_support(padapter, target_bw) &&
	    REGSTY_IS_BW_5G_SUPPORT(adapter_to_regsty(padapter), target_bw) &&
	    target_bw <= padapter->mlmeextpriv.cur_bwmode)
		ptdls_sta->bw_mode = target_bw;
	else
		ptdls_sta->bw_mode = padapter->mlmeextpriv.cur_bwmode;
	if (target_rxss != rtw_vht_mcsmap_to_nss(ptdls_sta->vhtpriv.vht_mcs_map)) {
		rtw_vht_nss_to_mcsmap(target_rxss, vht_mcs_map, ptdls_sta->vhtpriv.vht_mcs_map);
		_rtw_memcpy(ptdls_sta->vhtpriv.vht_mcs_map, vht_mcs_map, 2);
	}
}
