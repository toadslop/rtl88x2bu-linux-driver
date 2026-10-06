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
