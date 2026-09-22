// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-110 FT IE leaf (provenance: core/rtw_ft.c). */
#include "host_ft_types.h"

void host_ft_info_init(struct ft_roam_info *pft)
{
	_rtw_memset(pft, 0, sizeof(struct ft_roam_info));
	pft->ft_flags = 0 | RTW_FT_EN
#ifdef CONFIG_RTW_BTM_ROAM
			| RTW_FT_BTM_ROAM
#endif
		;
	pft->ft_updated_bcn = _FALSE;
}

u8 host_ft_update_rsnie(_adapter *padapter, u8 bwrite, struct pkt_attrib *pattrib,
			  u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie;
	s32 len;

	pie = rtw_get_ie(pft_roam->updated_ft_ies, EID_WPA2, &len,
			 pft_roam->updated_ft_ies_len);

	if (!bwrite)
		return (pie) ? _SUCCESS : _FAIL;

	if (pie) {
		*pframe = rtw_set_ie(((u8 *)*pframe), EID_WPA2, (uint)len, pie + 2,
				     &(pattrib->pktlen));
	} else {
		return _FAIL;
	}

	return _SUCCESS;
}

u8 host_ft_update_mdie(_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie, mdie[3];
	s32 len = 3;

	if (rtw_ft_roam(padapter)) {
		if ((pie = rtw_get_ie(pft_roam->updated_ft_ies, _MDIE_, &len,
				      pft_roam->updated_ft_ies_len))) {
			pie = (pie + 2);
		} else {
			return _FAIL;
		}
	} else {
		*((u16 *)&mdie[0]) = pft_roam->mdid;
		mdie[2] = pft_roam->ft_cap;
		pie = &mdie[0];
	}

	*pframe = rtw_set_ie(((u8 *)*pframe), _MDIE_, (uint)len, pie, &(pattrib->pktlen));
	return _SUCCESS;
}

u8 host_ft_update_ftie(_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie;
	s32 len;

	if ((pie = rtw_get_ie(pft_roam->updated_ft_ies, _FTIE_, &len,
			      pft_roam->updated_ft_ies_len)) != NULL) {
		*pframe = rtw_set_ie(*pframe, _FTIE_, (uint)len, (pie + 2),
				     &(pattrib->pktlen));
	} else {
		return _FAIL;
	}

	return _SUCCESS;
}

void host_ft_build_auth_req_ies(_adapter *padapter, struct pkt_attrib *pattrib,
				u8 **pframe)
{
	u8 ftie_append = _TRUE;

	if (!pattrib || !(*pframe))
		return;

	if (!rtw_ft_roam(padapter))
		return;

	ftie_append = host_ft_update_rsnie(padapter, _TRUE, pattrib, pframe);
	host_ft_update_mdie(padapter, pattrib, pframe);
	if (ftie_append)
		host_ft_update_ftie(padapter, pattrib, pframe);
}

void host_ft_build_assoc_req_ies(_adapter *padapter, u8 is_reassoc,
				 struct pkt_attrib *pattrib, u8 **pframe)
{
	if (!pattrib || !(*pframe))
		return;

	if (rtw_ft_chk_flags(padapter, RTW_FT_PEER_EN))
		host_ft_update_mdie(padapter, pattrib, pframe);

	if ((!is_reassoc) || (!rtw_ft_roam(padapter)))
		return;

	if (host_ft_update_rsnie(padapter, _FALSE, pattrib, pframe))
		host_ft_update_ftie(padapter, pattrib, pframe);
}

u8 host_ft_chk_roaming_candidate(_adapter *padapter, struct wlan_network *competitor)
{
	u8 *pmdie;
	s32 mdie_len = 0;
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);

	if (!(pmdie = rtw_get_ie(&competitor->network.IEs[12], _MDIE_, &mdie_len,
				(sint)(competitor->network.IELength - 12))))
		return _FALSE;

	if (!_rtw_memcmp(&pft_roam->mdid, (pmdie + 2), 2))
		return _FALSE;

	if (rtw_ft_valid_otd_candidate(padapter, pmdie))
		return _FALSE;

	if (rtw_ft_chk_flags(padapter, RTW_FT_TEST_RSSI_ROAM)) {
		if (!_rtw_memcmp(padapter->mlmepriv.cur_network.network.MacAddress,
				 competitor->network.MacAddress, ETH_ALEN)) {
			competitor->network.Rssi += 20;
			rtw_ft_clr_flags(padapter, RTW_FT_TEST_RSSI_ROAM);
		}
	}

	return _TRUE;
}

u8 host_ft_update_auth_rsp_ies(_adapter *padapter, u8 *pframe, u32 len)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);
	u8 target_ap_addr[ETH_ALEN] = {0};

	if (!rtw_ft_roam(padapter))
		return _FAIL;
	if (rtw_ft_authed_sta(padapter))
		return _SUCCESS;
	if (!pframe || !len)
		return _FAIL;

	rtw_buf_update(&pmlmepriv->auth_rsp, &pmlmepriv->auth_rsp_len, pframe, len);
	pft_roam->ft_event.ies =
		(pmlmepriv->auth_rsp + 24 + 6);
	pft_roam->ft_event.ies_len =
		(u16)(pmlmepriv->auth_rsp_len - 24 - 6);
	pft_roam->ft_event.ric_ies = NULL;
	pft_roam->ft_event.ric_ies_len = 0;
	_rtw_memcpy(target_ap_addr, pmlmepriv->assoc_bssid, ETH_ALEN);
	rtw_ft_report_reassoc_evt(padapter, target_ap_addr);

	return _SUCCESS;
}
