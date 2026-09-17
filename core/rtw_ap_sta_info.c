/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_STA_INFO_C_

#ifdef HOST_AP_STA_INFO_TEST
#include "host_ap_sta_info_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#ifdef CONFIG_AP_MODE

#if defined(CONFIG_80211N_HT) && defined(CONFIG_BEAMFORMING)
#if !defined(CONFIG_RUST_AP_STA_INFO) || defined(HOST_AP_STA_INFO_TEST)
void update_sta_info_apmode_ht_bf_cap(_adapter *padapter, struct sta_info *psta)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;
	struct ht_priv	*phtpriv_sta = &psta->htpriv;

	u8 cur_beamform_cap = 0;

	if (TEST_FLAG(phtpriv_ap->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE) &&
		GET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE);
		SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS((u8 *)(&phtpriv_sta->ht_cap)) << 6);
	}

	if (TEST_FLAG(phtpriv_ap->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE) &&
		GET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE);
		SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS((u8 *)(&phtpriv_sta->ht_cap)) << 4);
	}
#ifndef HOST_AP_STA_INFO_TEST
	if (cur_beamform_cap)
		RTW_INFO("Client STA(%d) HT Beamforming Cap = 0x%02X\n", psta->cmn.aid, cur_beamform_cap);
#endif

	phtpriv_sta->beamform_cap = cur_beamform_cap;
	psta->cmn.bf_info.ht_beamform_cap = cur_beamform_cap;

}
#endif /* !CONFIG_RUST_AP_STA_INFO || HOST_AP_STA_INFO_TEST */
#endif /* CONFIG_80211N_HT && CONFIG_BEAMFORMING */

#ifdef CONFIG_80211N_HT
void update_hw_ht_param(_adapter *padapter)
{
	unsigned char		max_AMPDU_len;
	unsigned char		min_MPDU_spacing;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

#ifndef HOST_AP_STA_INFO_TEST
	RTW_INFO("%s\n", __FUNCTION__);
#endif

	max_AMPDU_len = pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x03;

	min_MPDU_spacing = (pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c) >> 2;

	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_MIN_SPACE, (u8 *)(&min_MPDU_spacing));

	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_FACTOR, (u8 *)(&max_AMPDU_len));

	pmlmeinfo->SM_PS = (pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info & 0x0C) >> 2;
	if (pmlmeinfo->SM_PS == WLAN_HT_CAP_SM_PS_STATIC) {
#ifndef HOST_AP_STA_INFO_TEST
		RTW_INFO("%s(): WLAN_HT_CAP_SM_PS_STATIC\n", __FUNCTION__);
#endif
	}
}
#endif /* CONFIG_80211N_HT */

#endif /* CONFIG_AP_MODE */
