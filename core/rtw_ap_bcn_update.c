// SPDX-License-Identifier: GPL-2.0
#define _RTW_AP_BCN_UPDATE_C_

#ifdef HOST_AP_BCN_UPDATE_TEST
#include "host_ap_bcn_update_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_BCN_UPDATE) || defined(HOST_AP_BCN_UPDATE_TEST)

#ifdef HOST_AP_BCN_UPDATE_TEST
extern u8 host_bcn_update_last_erp_byte;
extern u16 host_bcn_update_last_ht_op_mode;
extern u8 host_bcn_update_last_ht_info_byte;
extern u32 host_bcn_update_last_ielen;

void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE)
{
	(void)padapter;
	if (pIE && pIE->Length > 0)
		host_bcn_update_last_erp_byte = pIE->data[0];
}
#endif

void update_bcn_erpinfo_ie(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
#ifdef HOST_AP_BCN_UPDATE_TEST
	sint len = 0;
#else
	u32 len = 0;
#endif

#ifndef HOST_AP_BCN_UPDATE_TEST
	RTW_INFO("%s, ERP_enable=%d\n", __FUNCTION__, pmlmeinfo->ERP_enable);
#endif

	if (!pmlmeinfo->ERP_enable)
		return;

	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _ERPINFO_IE_, &len,
		       (pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		PNDIS_802_11_VARIABLE_IEs pIE = (PNDIS_802_11_VARIABLE_IEs)p;

		if (pmlmepriv->num_sta_non_erp == 1)
			pIE->data[0] |= RTW_ERP_INFO_NON_ERP_PRESENT |
					 RTW_ERP_INFO_USE_PROTECTION;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_NON_ERP_PRESENT |
					  RTW_ERP_INFO_USE_PROTECTION);

		if (pmlmepriv->num_sta_no_short_preamble > 0)
			pIE->data[0] |= RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_BARKER_PREAMBLE_MODE);

		ERP_IE_handler(padapter, pIE);
	}
}

#ifdef CONFIG_80211N_HT
void update_bcn_htinfo_ie(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
#ifdef HOST_AP_BCN_UPDATE_TEST
	sint len = 0;
#else
	u32 len = 0;
#endif

	if (pmlmepriv->htpriv.ht_option == _FALSE)
		return;
	if (pmlmeinfo->HT_info_enable != 1)
		return;

#ifndef HOST_AP_BCN_UPDATE_TEST
	RTW_INFO("%s current operation mode=0x%X\n",
		 __FUNCTION__, pmlmepriv->ht_op_mode);
#endif

	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_ADD_INFO_IE_, &len,
		       (sint)(pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		struct HT_info_element *pht_info =
			(struct HT_info_element *)(p + 2);

		if ((pmlmepriv->sw_to_20mhz == 0) && (pmlmeext->cur_channel <= 14)) {
			if ((pmlmepriv->num_sta_40mhz_intolerant > 0) ||
			    (pmlmepriv->ht_20mhz_width_req == _TRUE) ||
			    (pmlmepriv->ht_intolerant_ch_reported == _TRUE) ||
#ifdef HOST_AP_BCN_UPDATE_TEST
			    (pmlmepriv->olbc > 0)
#else
			    (ATOMIC_READ(&pmlmepriv->olbc) == _TRUE)
#endif
			   ) {
				SET_HT_OP_ELE_2ND_CHL_OFFSET(pht_info, 0);
				SET_HT_OP_ELE_STA_CHL_WIDTH(pht_info, 0);
				pmlmepriv->sw_to_20mhz = 1;
			}
		} else if ((pmlmepriv->num_sta_40mhz_intolerant == 0) &&
			   (pmlmepriv->ht_20mhz_width_req == _FALSE) &&
			   (pmlmepriv->ht_intolerant_ch_reported == _FALSE) &&
#ifdef HOST_AP_BCN_UPDATE_TEST
			   (pmlmepriv->olbc == 0)
#else
			   (ATOMIC_READ(&pmlmepriv->olbc) == _FALSE)
#endif
			  ) {
			if (pmlmeext->cur_bwmode >= CHANNEL_WIDTH_40) {
				SET_HT_OP_ELE_STA_CHL_WIDTH(pht_info, 1);
				SET_HT_OP_ELE_2ND_CHL_OFFSET(
					pht_info,
					(pmlmeext->cur_ch_offset ==
					 HAL_PRIME_CHNL_OFFSET_LOWER) ?
						HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE :
						HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW);
				pmlmepriv->sw_to_20mhz = 0;
			}
		}

		*(u16 *)(pht_info->infos + 1) =
			cpu_to_le16(pmlmepriv->ht_op_mode);
#ifdef HOST_AP_BCN_UPDATE_TEST
		host_bcn_update_last_ht_info_byte = pht_info->infos[0];
		host_bcn_update_last_ht_op_mode =
			(u16)pht_info->infos[1] |
			((u16)pht_info->infos[2] << 8);
#endif
	}
}
#endif /* CONFIG_80211N_HT */

void update_bcn_wps_ie(_adapter *padapter)
{
	u8 *pwps_ie = NULL, *pwps_ie_src, *premainder_ie, *pbackup_remainder_ie = NULL;
#ifdef HOST_AP_BCN_UPDATE_TEST
	u32 wps_ielen = 0, wps_offset, remainder_ielen;
#else
	uint wps_ielen = 0, wps_offset, remainder_ielen;
#endif
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	u8 *ie = pnetwork->IEs;
	u32 ielen = pnetwork->IELength;

#ifndef HOST_AP_BCN_UPDATE_TEST
	RTW_INFO("%s\n", __FUNCTION__);
#endif

	pwps_ie = rtw_get_wps_ie(ie + _FIXED_IE_LENGTH_, ielen - _FIXED_IE_LENGTH_,
				 NULL, &wps_ielen);
	if (pwps_ie == NULL || wps_ielen == 0)
		return;

	pwps_ie_src = pmlmepriv->wps_beacon_ie;
	if (pwps_ie_src == NULL)
		return;

	wps_offset = (u32)(pwps_ie - ie);
	premainder_ie = pwps_ie + wps_ielen;
	remainder_ielen = ielen - wps_offset - wps_ielen;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	wps_ielen = (u32)pwps_ie_src[1];
	if ((wps_offset + wps_ielen + 2 + remainder_ielen) <= MAX_IE_SZ) {
		_rtw_memcpy(pwps_ie, pwps_ie_src, wps_ielen + 2);
		pwps_ie += (wps_ielen + 2);
		if (pbackup_remainder_ie)
			_rtw_memcpy(pwps_ie, pbackup_remainder_ie, remainder_ielen);
		pnetwork->IELength = wps_offset + (wps_ielen + 2) + remainder_ielen;
	}

	if (pbackup_remainder_ie)
		rtw_mfree(pbackup_remainder_ie, remainder_ielen);

#ifdef HOST_AP_BCN_UPDATE_TEST
	host_bcn_update_last_ielen = pnetwork->IELength;
#else
#if defined(CONFIG_INTERRUPT_BASED_TXBCN) || defined(CONFIG_PCI_HCI)
	if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
		u8 sr = 0;

		rtw_get_wps_attr_content(pwps_ie_src, wps_ielen,
					 WPS_ATTR_SELECTED_REGISTRAR, (u8 *)(&sr), NULL);
		if (sr) {
			set_fwstate(pmlmepriv, WIFI_UNDER_WPS);
			RTW_INFO("%s, set WIFI_UNDER_WPS\n", __func__);
		} else {
			clr_fwstate(pmlmepriv, WIFI_UNDER_WPS);
			RTW_INFO("%s, clr WIFI_UNDER_WPS\n", __func__);
		}
	}
#endif
#endif /* HOST_AP_BCN_UPDATE_TEST */
}

#endif /* !CONFIG_RUST_AP_BCN_UPDATE || HOST_AP_BCN_UPDATE_TEST */
