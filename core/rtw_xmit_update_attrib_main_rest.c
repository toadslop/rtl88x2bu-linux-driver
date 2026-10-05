/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _RTW_XMIT_UPDATE_ATTRIB_MAIN_REST_C_

#include <drv_types.h>

u8 tos_to_up(u8 tos);

void rtw_xmit_update_attrib_set_qos(_pkt *pkt, struct pkt_attrib *pattrib)
{
	s32 UserPriority = 0;

	if (!pkt)
		goto null_pkt;

	/* get UserPriority from IP hdr */
	if (pattrib->ether_type == 0x0800) {
		struct pkt_file ppktfile;
		struct ethhdr etherhdr;
		struct iphdr ip_hdr;

		_rtw_open_pktfile(pkt, &ppktfile);
		_rtw_pktfile_read(&ppktfile, (unsigned char *)&etherhdr, ETH_HLEN);
		_rtw_pktfile_read(&ppktfile, (u8 *)&ip_hdr, sizeof(ip_hdr));
		UserPriority = tos_to_up(ip_hdr.tos);
	}

	#ifdef CONFIG_ICMP_VOQ
	if (pattrib->icmp_pkt == 1)
		UserPriority = 7;
	#endif
	#ifdef CONFIG_IP_R_MONITOR
	if (pattrib->ether_type == ETH_P_ARP)
		UserPriority = 7;
	#endif

null_pkt:
	pattrib->priority = UserPriority;
	pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_QOS_LEN : WLAN_HDR_A3_QOS_LEN;
	pattrib->subtype = WIFI_QOS_DATA_TYPE;
}

#ifdef CONFIG_LPS
#define LPS_PT_NORMAL	0
#define LPS_PT_SP		1
#define LPS_PT_ICMP		2

u8 rtw_xmit_update_attrib_lps_chk_packet_type(struct pkt_attrib *pattrib)
{
	u8 pkt_type = LPS_PT_NORMAL;

	#ifdef CONFIG_WAPI_SUPPORT
	if ((pattrib->ether_type == 0x88B4) || (pattrib->ether_type == 0x0806) ||
	    (pattrib->ether_type == 0x888e) || (pattrib->dhcp_pkt == 1))
		pkt_type = LPS_PT_SP;
	#else

	#ifndef CONFIG_LPS_NOT_LEAVE_FOR_ICMP
	if (pattrib->icmp_pkt == 1)
		pkt_type = LPS_PT_ICMP;
	else
	#endif
		if (pattrib->dhcp_pkt == 1)
			pkt_type = LPS_PT_SP;
	#endif
	return pkt_type;
}
#endif /* CONFIG_LPS */

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe);
void update_attrib_phy_info(_adapter *padapter, struct pkt_attrib *pattrib,
			    struct sta_info *psta);
s32 update_attrib_sec_info(_adapter *padapter, struct pkt_attrib *pattrib,
			   struct sta_info *psta, enum eap_type eapol_type);
#ifdef CONFIG_WMMPS_STA
void update_attrib_trigger_frame_info(_adapter *padapter, struct pkt_attrib *pattrib);
#endif
#ifdef CONFIG_BEAMFORMING
void update_attrib_txbf_info(_adapter *padapter, struct pkt_attrib *pattrib,
			     struct sta_info *psta);
#endif

s32 rtw_xmit_update_attrib_post_l4(_adapter *padapter, _pkt *pkt,
				   struct pkt_attrib *pattrib, struct sta_info *psta,
				   struct mlme_priv *pmlmepriv, struct qos_priv *pqospriv,
				   struct xmit_priv *pxmitpriv, sint bmcast,
				   enum eap_type eapol_type)
{
	sint res = _SUCCESS;
#ifdef CONFIG_LPS
	u8 pkt_type;
#define LPS_PT_NORMAL	0
#define LPS_PT_SP		1
#define LPS_PT_ICMP		2
#endif

	if ((pattrib->ether_type == 0x888e) || (pattrib->dhcp_pkt == 1))
		rtw_mi_set_scan_deny(padapter, 3000);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) &&
	    pattrib->ether_type == ETH_P_ARP &&
	    !IS_MCAST(pattrib->dst)) {
		rtw_mi_set_scan_deny(padapter, 1000);
		rtw_mi_scan_abort(padapter, _FALSE);
	}

#ifdef CONFIG_LPS
	pkt_type = rtw_xmit_update_attrib_lps_chk_packet_type(pattrib);

	if (pkt_type == LPS_PT_SP) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_active);
		rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_SPECIAL_PACKET, 0);
	} else if (pkt_type == LPS_PT_ICMP)
		rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_LEAVE, 0);
#endif

#ifdef CONFIG_BEAMFORMING
	update_attrib_txbf_info(padapter, pattrib, psta);
#endif

	if (update_attrib_sec_info(padapter, pattrib, psta, eapol_type) == _FAIL) {
		DBG_COUNTER(padapter->tx_logs.core_tx_upd_attrib_err_sec);
		return _FAIL;
	}

	pattrib->pkt_hdrlen = ETH_HLEN;
	pattrib->hdrlen = XATTRIB_GET_WDS(pattrib) ? WLAN_HDR_A4_LEN : WLAN_HDR_A3_LEN;
	pattrib->subtype = WIFI_DATA_TYPE;
	pattrib->qos_en = psta->qos_option;
	pattrib->priority = 0;

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE | WIFI_MESH_STATE
	    | WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE)) {
		if (pattrib->qos_en) {
			rtw_xmit_update_attrib_set_qos(pkt, pattrib);
			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter))
				rtw_mesh_tx_set_whdr_mctrl_len(pattrib->mesh_frame_mode, pattrib);
			#endif
		}
	} else {
#ifdef CONFIG_TDLS
		if (pattrib->direct_link == _TRUE) {
			if (pattrib->qos_en)
				rtw_xmit_update_attrib_set_qos(pkt, pattrib);
		} else
#endif
		{
			if (pqospriv->qos_option) {
				rtw_xmit_update_attrib_set_qos(pkt, pattrib);

				if (pmlmepriv->acm_mask != 0)
					pattrib->priority = qos_acm(pmlmepriv->acm_mask, pattrib->priority);
			}
		}
	}

	update_attrib_phy_info(padapter, pattrib, psta);

	pattrib->psta = psta;

#ifdef CONFIG_AUTO_AP_MODE
	if (psta->isrc && psta->pid > 0)
		pattrib->pctrl = _TRUE;
	else
#endif
		pattrib->pctrl = 0;

	pattrib->ack_policy = 0;

	if (bmcast)
		pattrib->rate = psta->init_rate;

#ifdef CONFIG_WMMPS_STA
	update_attrib_trigger_frame_info(padapter, pattrib);
#endif

	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;
	rtw_set_tx_chksum_offload(pkt, pattrib);

	return res;
}

#ifdef CONFIG_WMMPS_STA
/*
 * update_attrib_trigger_frame_info
 * For Station mode, if a specific TID of driver setting and an AP support uapsd function, the data
 * frame with corresponding TID will be a trigger frame when driver is in wmm power saving mode.
 */
void update_attrib_trigger_frame_info(_adapter *padapter, struct pkt_attrib *pattrib)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct qos_priv *pqospriv = &pmlmepriv->qospriv;
	u8 trigger_frame_en = 0;

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE) {
		if ((pwrpriv->pwr_mode == PS_MODE_MIN) || (pwrpriv->pwr_mode == PS_MODE_MAX)) {
			if ((pqospriv->uapsd_ap_supported) &&
			    ((pqospriv->uapsd_tid & BIT(pattrib->priority)) == _TRUE)) {
				trigger_frame_en = 1;
				RTW_INFO("[WMMPS]" FUNC_ADPT_FMT ": This is a Trigger Frame\n",
					 FUNC_ADPT_ARG(padapter));
			}
		}
	}

	pattrib->trigger_frame = trigger_frame_en;
}
#endif /* CONFIG_WMMPS_STA */
