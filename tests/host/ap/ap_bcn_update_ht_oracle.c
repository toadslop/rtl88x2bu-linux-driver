// SPDX-License-Identifier: GPL-2.0
#include "host_ap_bcn_update_types.h"

u16 host_bcn_update_last_ht_op_mode;

void update_bcn_htinfo_ie(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
	sint len = 0;

	if (pmlmepriv->htpriv.ht_option == _FALSE)
		return;
	if (pmlmeinfo->HT_info_enable != 1)
		return;

	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_ADD_INFO_IE_, &len,
		       (sint)(pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		struct HT_info_element *pht_info =
			(struct HT_info_element *)(p + 2);

		if ((pmlmepriv->sw_to_20mhz == 0) && (pmlmeext->cur_channel <= 14)) {
			if ((pmlmepriv->num_sta_40mhz_intolerant > 0) ||
			    (pmlmepriv->ht_20mhz_width_req == _TRUE) ||
			    (pmlmepriv->ht_intolerant_ch_reported == _TRUE) ||
			    (pmlmepriv->olbc != 0)) {
				SET_HT_OP_ELE_2ND_CHL_OFFSET(pht_info, 0);
				SET_HT_OP_ELE_STA_CHL_WIDTH(pht_info, 0);
				pmlmepriv->sw_to_20mhz = 1;
			}
		} else if ((pmlmepriv->num_sta_40mhz_intolerant == 0) &&
			   (pmlmepriv->ht_20mhz_width_req == _FALSE) &&
			   (pmlmepriv->ht_intolerant_ch_reported == _FALSE) &&
			   (pmlmepriv->olbc == 0)) {
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
		host_bcn_update_last_ht_op_mode =
			(u16)pht_info->infos[1] |
			((u16)pht_info->infos[2] << 8);
	}
}
