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

u16 rtw_ap_sta_sec_apply_policy_wps(_adapter *adapter, struct sta_info *sta,
				    struct security_priv *sec,
				    struct rtw_ieee802_11_elems *elems,
				    u8 *wpa_ie, int wpa_ie_len, int gmcs,
				    u8 mfp_opt, u8 spp_opt)
{
	u16 status = _STATS_SUCCESSFUL_;

	if (sec->dot11PrivacyAlgrthm != _NO_PRIVACY_) {
		if (rtw_check_amsdu_disable(adapter->registrypriv.amsdu_mode, spp_opt) == _TRUE)
			sta->flags |= WLAN_STA_AMSDU_DISABLE;
	}

	if ((sec->mfp_opt == MFP_REQUIRED && mfp_opt < MFP_OPTIONAL)
		|| (mfp_opt == MFP_REQUIRED && sec->mfp_opt < MFP_OPTIONAL)
	) {
		status = WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION;
		goto exit;
	}

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		if (adapter->mesh_info.mesh_auth_id)
			sta->flags |= WLAN_STA_MFP;
	} else
#endif
	if (sec->mfp_opt >= MFP_OPTIONAL && mfp_opt >= MFP_OPTIONAL)
		sta->flags |= WLAN_STA_MFP;

#ifdef CONFIG_IEEE80211W
	if ((sta->flags & WLAN_STA_MFP)
		&& (sec->mfp_opt >= MFP_OPTIONAL && mfp_opt >= MFP_OPTIONAL)
		&& security_type_bip_to_gmcs(sec->dot11wCipher) != gmcs
	) {
		status = WLAN_STATUS_CIPHER_REJECTED_PER_POLICY;
		goto exit;
	}
#endif

#ifdef CONFIG_IOCTL_CFG80211
	if (MLME_IS_AP(adapter) &&
		(sec->auth_type == MLME_AUTHTYPE_SAE) &&
		(CHECK_BIT(WLAN_AKM_TYPE_SAE, sta->akm_suite_type)) &&
		(WLAN_AUTH_OPEN == sta->authalg)) {
		if (rtw_cached_pmkid(adapter, sta->cmn.mac_addr) == -1) {
			RTW_INFO("SAE: No PMKSA cache entry found\n");
			status = WLAN_STATUS_INVALID_PMKID;
			goto exit;
		}
		RTW_INFO("SAE: PMKSA cache entry found\n");
	}
#endif

	if (!MLME_IS_AP(adapter))
		goto exit;

	sta->flags &= ~(WLAN_STA_WPS | WLAN_STA_MAYBE_WPS);
	if (wpa_ie == NULL) {
		if (elems->wps_ie) {
			RTW_INFO("STA included WPS IE in "
				 "(Re)Association Request - assume WPS is "
				 "used\n");
			sta->flags |= WLAN_STA_WPS;
		} else {
			RTW_INFO("STA did not include WPA/RSN IE "
				 "in (Re)Association Request - possible WPS "
				 "use\n");
			sta->flags |= WLAN_STA_MAYBE_WPS;
		}

		if ((sec->wpa_psk > 0)
			&& (sta->flags & (WLAN_STA_WPS | WLAN_STA_MAYBE_WPS))
		) {
			struct mlme_priv *mlme = &adapter->mlmepriv;

			if (mlme->wps_beacon_ie) {
				u8 selected_registrar = 0;

				rtw_get_wps_attr_content(mlme->wps_beacon_ie, mlme->wps_beacon_ie_len, WPS_ATTR_SELECTED_REGISTRAR, &selected_registrar, NULL);

				if (!selected_registrar) {
					RTW_INFO("selected_registrar is _FALSE , or AP is not ready to do WPS\n");
					status = _STATS_UNABLE_HANDLE_STA_;
					goto exit;
				}
			}
		}

	} else {
		int copy_len;

		if (sec->wpa_psk == 0) {
			RTW_INFO("STA " MAC_FMT
				": WPA/RSN IE in association request, but AP don't support WPA/RSN\n",
				MAC_ARG(sta->cmn.mac_addr));
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}

		if (elems->wps_ie) {
			RTW_INFO("STA included WPS IE in "
				 "(Re)Association Request - WPS is "
				 "used\n");
			sta->flags |= WLAN_STA_WPS;
			copy_len = 0;
		} else
			copy_len = ((wpa_ie_len + 2) > sizeof(sta->wpa_ie)) ? (sizeof(sta->wpa_ie)) : (wpa_ie_len + 2);

		if (copy_len > 0)
			_rtw_memcpy(sta->wpa_ie, wpa_ie - 2, copy_len);
	}

exit:
	return status;
}
