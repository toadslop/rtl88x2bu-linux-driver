/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_RECV_STA_REST_C_

#ifdef HOST_RECV_STA_TEST
#include "host_recv_sta_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST_RECV_STA) || defined(HOST_RECV_STA_TEST)

void count_rx_stats(_adapter *padapter, union recv_frame *prframe, struct sta_info *sta)
{
	int	sz;
	struct sta_info		*psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct rx_pkt_attrib	*pattrib = &prframe->u.hdr.attrib;
	struct recv_priv		*precvpriv = &padapter->recvpriv;

	sz = get_recvframe_len(prframe);
	precvpriv->rx_bytes += sz;

	padapter->mlmepriv.LinkDetectInfo.NumRxOkInPeriod++;

	if ((!MacAddr_isBcst(pattrib->dst)) && (!IS_MCAST(pattrib->dst)))
		padapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod++;

	if (sta)
		psta = sta;
	else
		psta = prframe->u.hdr.psta;

	if (psta) {
		u8 is_ra_bmc = IS_MCAST(pattrib->ra);

		pstats = &psta->sta_stats;

		pstats->last_rx_time = rtw_get_current_time();
		pstats->rx_data_pkts++;
		pstats->rx_bytes += sz;
		if (is_broadcast_mac_addr(pattrib->ra)) {
			pstats->rx_data_bc_pkts++;
			pstats->rx_bc_bytes += sz;
		} else if (is_ra_bmc) {
			pstats->rx_data_mc_pkts++;
			pstats->rx_mc_bytes += sz;
		}

		if (!is_ra_bmc) {
			pstats->rxratecnt[pattrib->data_rate]++;
			/*record rx packets for every tid*/
			pstats->rx_data_qos_pkts[pattrib->priority]++;
		}
#ifdef CONFIG_DYNAMIC_SOML
		rtw_dyn_soml_byte_update(padapter, pattrib->data_rate, sz);
#endif
#if defined(CONFIG_CHECK_LEAVE_LPS) && defined(CONFIG_LPS_CHK_BY_TP)
		if (adapter_to_pwrctl(padapter)->lps_chk_by_tp)
			traffic_check_for_leave_lps_by_tp(padapter, _FALSE, psta);
#endif /* CONFIG_LPS */

	}

#ifdef CONFIG_CHECK_LEAVE_LPS
#ifdef CONFIG_LPS_CHK_BY_TP
	if (!adapter_to_pwrctl(padapter)->lps_chk_by_tp)
#endif
		traffic_check_for_leave_lps(padapter, _FALSE, 0);
#endif /* CONFIG_CHECK_LEAVE_LPS */

}

int rtw_sta_rx_data_validate_hdr(_adapter *adapter, union recv_frame *rframe, struct sta_info **sta)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u8 *mybssid  = get_bssid(&adapter->mlmepriv);
	u8 *myhwaddr = adapter_mac_addr(adapter);
	struct rx_pkt_attrib *rattrib = &rframe->u.hdr.attrib;
	u8 *whdr = get_recvframe_data(rframe);
	u8 is_ra_bmc = IS_MCAST(GetAddr1Ptr(whdr)) ? 1 : 0;
	sint ret = _FAIL;

	(void)mybssid;

	if (rattrib->to_fr_ds == 0) {
		_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->dst, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->src, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->bssid, GetAddr3Ptr(whdr), ETH_ALEN);

		#ifdef CONFIG_TDLS
		if (adapter->tdlsinfo.link_established == _TRUE)
			ret = rtw_tdls_rx_data_validate_hdr(adapter, rframe, sta);
		else
		#endif
		{
			if (!_rtw_memcmp(rattrib->bssid, rattrib->src, ETH_ALEN))
				goto exit;

			*sta = rtw_get_stainfo(stapriv, get_addr2_ptr(whdr));
			if (*sta)
				ret = _SUCCESS;
		}
		goto exit;
	}

	if (!(MLME_STATE(adapter) & (WIFI_ASOC_STATE | WIFI_UNDER_LINKING))) {
		if (!is_ra_bmc) {
			static systime send_issue_deauth_time = 0;

			if (rtw_get_passing_time_ms(send_issue_deauth_time) > 10000 || send_issue_deauth_time == 0) {
				send_issue_deauth_time = rtw_get_current_time();
				RTW_INFO(FUNC_ADPT_FMT" issue_deauth to "MAC_FMT" with reason(7), mlme_state:0x%x\n"
					, FUNC_ADPT_ARG(adapter), MAC_ARG(get_addr2_ptr(whdr)), MLME_STATE(adapter));
				issue_deauth(adapter, get_addr2_ptr(whdr), WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
			}
		}
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" fw_state:0x%x\n"
			, FUNC_ADPT_ARG(adapter), MLME_STATE(adapter));
		#endif
		goto exit;
	}

	_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
	_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);

	switch (rattrib->to_fr_ds) {
	case 2:
		_rtw_memcpy(rattrib->dst, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->src, GetAddr3Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->bssid, get_addr2_ptr(whdr), ETH_ALEN);
		break;
	case 3:
		_rtw_memcpy(rattrib->dst, GetAddr3Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->src, GetAddr4Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->bssid, get_addr2_ptr(whdr), ETH_ALEN);
		break;
	default:
		ret = RTW_RX_HANDLED;
		goto exit;
	}

	if (!rattrib->amsdu && _rtw_memcmp(myhwaddr, rattrib->src, ETH_ALEN))
		goto exit;

	*sta = rtw_get_stainfo(stapriv, rattrib->ta);
	if (*sta == NULL) {
		#ifndef CONFIG_CUSTOMER_ALIBABA_GENERAL
		if (!is_ra_bmc && !IS_RADAR_DETECTED(adapter_to_rfctl(adapter))) {
			RTW_INFO(FUNC_ADPT_FMT" issue_deauth to "MAC_FMT" with reason(7), unknown TA\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(rattrib->ta));
			issue_deauth(adapter, rattrib->ta, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
		}
		#endif
		goto exit;
	}

#ifdef CONFIG_RTW_WDS_AUTO_EN
	if (rattrib->to_fr_ds == 3 && !((*sta)->flags & WLAN_STA_WDS))
		(*sta)->flags |= WLAN_STA_WDS;
#endif

	if (get_frame_sub_type(whdr) & BIT(6)) {
		count_rx_stats(adapter, rframe, *sta);
		ret = RTW_RX_HANDLED;
		goto exit;
	}

#ifdef CONFIG_RTW_WDS
	if (adapter_use_wds(adapter)
		&& !rattrib->amsdu && IS_MCAST(rattrib->dst)
		&& rtw_rx_wds_gptr_check(adapter, rattrib->src)
	) {
		count_rx_stats(adapter, rframe, *sta);
		ret = RTW_RX_HANDLED;
		goto exit;
	}
#endif

	ret = _SUCCESS;

exit:
	return ret;
}

#endif /* !CONFIG_RUST_RECV_STA || HOST_RECV_STA_TEST */
