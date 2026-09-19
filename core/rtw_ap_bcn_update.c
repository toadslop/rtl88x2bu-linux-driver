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

#endif /* !CONFIG_RUST_AP_BCN_UPDATE || HOST_AP_BCN_UPDATE_TEST */
