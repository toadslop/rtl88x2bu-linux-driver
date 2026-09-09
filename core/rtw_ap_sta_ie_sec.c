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
#define _RTW_AP_STA_IE_SEC_C_

#include <drv_types.h>
#include <hal_data.h>

u16 rtw_ap_sta_sec_parse_cipher_ies(_adapter *adapter, struct sta_info *sta,
				    struct security_priv *sec,
				    struct rtw_ieee802_11_elems *elems,
				    u8 **wpa_ie, int *wpa_ie_len,
				    int *group_cipher, int *pairwise_cipher,
				    int *gmcs, u32 *akm, u8 *mfp_opt, u8 *spp_opt)
{
	u16 status = _STATS_SUCCESSFUL_;

	*group_cipher = 0;
	*pairwise_cipher = 0;
	*gmcs = 0;
	*akm = 0;
	*mfp_opt = MFP_NO;
	*spp_opt = 0;

	if ((sec->wpa_psk & BIT(1)) && elems->rsn_ie) {
		*wpa_ie = elems->rsn_ie;
		*wpa_ie_len = elems->rsn_ie_len;

		if (rtw_parse_wpa2_ie(*wpa_ie - 2, *wpa_ie_len + 2, group_cipher,
				      pairwise_cipher, gmcs, akm, mfp_opt,
				      spp_opt) == _SUCCESS) {
			sta->dot8021xalg = 1;/* psk, todo:802.1x */
			sta->wpa_psk |= BIT(1);

			sta->wpa2_group_cipher = *group_cipher & sec->wpa2_group_cipher;
			sta->wpa2_pairwise_cipher = *pairwise_cipher & sec->wpa2_pairwise_cipher;

			sta->akm_suite_type = *akm;
			if (MLME_IS_AP(adapter) &&
			    (CHECK_BIT(WLAN_AKM_TYPE_SAE, *akm)) &&
			    (MFP_NO == *mfp_opt)) {
				status = WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION;
				goto exit;
			}

			if (MLME_IS_AP(adapter) && (!CHECK_BIT(sec->akmp, *akm))) {
				status = WLAN_STATUS_AKMP_NOT_VALID;
				goto exit;
			}

			if (!sta->wpa2_group_cipher) {
				status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
				goto exit;
			}

			if (!sta->wpa2_pairwise_cipher) {
				status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
				goto exit;
			}

		} else {
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}

	} else if ((sec->wpa_psk & BIT(0)) && elems->wpa_ie) {
		*wpa_ie = elems->wpa_ie;
		*wpa_ie_len = elems->wpa_ie_len;

		if (rtw_parse_wpa_ie(*wpa_ie - 2, *wpa_ie_len + 2, group_cipher,
				     pairwise_cipher, NULL) == _SUCCESS) {
			sta->dot8021xalg = 1;/* psk, todo:802.1x */
			sta->wpa_psk |= BIT(0);

			sta->wpa_group_cipher = *group_cipher & sec->wpa_group_cipher;
			sta->wpa_pairwise_cipher = *pairwise_cipher & sec->wpa_pairwise_cipher;

			if (!sta->wpa_group_cipher) {
				status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
				goto exit;
			}

			if (!sta->wpa_pairwise_cipher) {
				status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
				goto exit;
			}
		} else {
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}

	} else {
		*wpa_ie = NULL;
		*wpa_ie_len = 0;
	}

exit:
	return status;
}
