// SPDX-License-Identifier: GPL-2.0
/* Oracle copy of update_bcn_erpinfo_ie from core/rtw_ap_bcn_update.c — keep in sync to prevent drift. */
#include "host_ap_bcn_update_types.h"
#include <stdlib.h>
#include <string.h>

u8 host_bcn_update_last_erp_byte;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp, i;
	const u8 *p;

	if (limit < 1)
		return NULL;
	p = pbuf;
	for (i = 0, *len = 0;;) {
		if (*p == index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += (tmp + 2);
		i += (tmp + 2);
		if (i >= limit)
			break;
	}
	return NULL;
}

void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE)
{
	(void)padapter;
	if (pIE && pIE->Length > 0)
		host_bcn_update_last_erp_byte = pIE->data[0];
}

void update_bcn_erpinfo_ie(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
	sint len = 0;

	if (!pmlmeinfo->ERP_enable)
		return;

	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _ERPINFO_IE_, &len,
		       (sint)(pnetwork->IELength - _BEACON_IE_OFFSET_));
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
			pIE->data[0] &= ~RTW_ERP_INFO_BARKER_PREAMBLE_MODE;

		ERP_IE_handler(padapter, pIE);
	}
}

u16 host_bcn_update_last_ht_op_mode;
u8 host_bcn_update_last_ht_info_byte;

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
			    (pmlmepriv->olbc > 0)) {
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
		host_bcn_update_last_ht_info_byte = pht_info->infos[0];
		host_bcn_update_last_ht_op_mode =
			(u16)pht_info->infos[1] |
			((u16)pht_info->infos[2] << 8);
	}
}

u32 host_bcn_update_last_ielen;

void *rtw_malloc(size_t sz)
{
	return malloc(sz);
}

void rtw_mfree(void *p, size_t sz)
{
	(void)sz;
	free(p);
}

u8 *rtw_get_wps_ie(const u8 *in_ie, u32 in_len, u8 *wps_ie, u32 *wps_ielen)
{
	u32 cnt = 0;
	const u8 wps_oui[4] = {0x00, 0x50, 0xf2, 0x04};

	if (wps_ielen)
		*wps_ielen = 0;
	if (!in_ie || in_len <= 0)
		return NULL;

	while (cnt + 1 + 4 < in_len) {
		u8 eid = in_ie[cnt];

		if (eid == WLAN_EID_VENDOR_SPECIFIC &&
		    memcmp(&in_ie[cnt + 2], wps_oui, 4) == 0) {
			if (wps_ielen)
				*wps_ielen = in_ie[cnt + 1] + 2;
			if (wps_ie)
				_rtw_memcpy(wps_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);
			return (u8 *)(in_ie + cnt);
		}
		cnt += in_ie[cnt + 1] + 2;
	}
	return NULL;
}

void update_bcn_wps_ie(_adapter *padapter)
{
	u8 *pwps_ie = NULL, *pwps_ie_src, *premainder_ie, *pbackup_remainder_ie = NULL;
	u32 wps_ielen = 0, wps_offset, remainder_ielen;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	u8 *ie = pnetwork->IEs;
	u32 ielen = pnetwork->IELength;

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

	host_bcn_update_last_ielen = pnetwork->IELength;
}
