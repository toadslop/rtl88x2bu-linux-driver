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
#define _RTW_MLME_EXT_REST_C_

#if defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST)
#include "host_mlme_ext_mgnt_attrib_types.h"
#undef rtw_warn_on
extern int rtw_warn_on(int cond);
#elif defined(HOST_MLME_EXT_PEER_ALIVE_TEST)
#include "host_mlme_ext_peer_alive_types.h"
#elif defined(HOST_MLME_EXT_TEST)
#include "host_mlme_ext_types.h"
#undef rtw_warn_on
extern int rtw_warn_on(int cond);
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if (!defined(CONFIG_RUST) || defined(HOST_MLME_EXT_TEST) || \
      !defined(CONFIG_RUST_MLME_EXT_REST)) && \
     !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) && \
     !defined(HOST_MLME_EXT_PEER_ALIVE_TEST)

#ifdef CONFIG_DFS_MASTER
bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	bool ret = _FALSE;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			ret = _TRUE;
			break;
		}
	}

exit:
	return ret;
}

bool rtw_chset_is_ch_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch)
{
	return rtw_chset_is_chbw_non_ocp(ch_set, ch, CHANNEL_WIDTH_20,
					 HAL_PRIME_CHNL_OFFSET_DONT_CARE);
}

u32 rtw_chset_get_ch_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	int ms = 0;
	systime current_time;
	u32 hi = 0, lo = 0;
	int i;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	current_time = rtw_get_current_time();

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (!CH_IS_NON_OCP(&ch_set[i]))
			continue;

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if ((int)rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						   current_time) > ms)
				ms = rtw_systime_to_ms(ch_set[i].non_ocp_end_time -
						       current_time);
		}
	}

exit:
	return ms;
}

static bool _rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				      u8 offset, int ms)
{
	u32 hi = 0, lo = 0;
	int i;
	bool updated = 0;

	if (rtw_chbw_to_freq_range(ch, bw, offset, &hi, &lo) == _FALSE)
		goto exit;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (!rtw_ch2freq(ch_set[i].ChannelNum)) {
			rtw_warn_on(1);
			continue;
		}

		if (lo <= (u32)rtw_ch2freq(ch_set[i].ChannelNum)
		    && (u32)rtw_ch2freq(ch_set[i].ChannelNum) <= hi) {
			if (ms >= 0)
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(ms);
			else
				ch_set[i].non_ocp_end_time =
					rtw_get_current_time() +
					rtw_ms_to_systime(NON_OCP_TIME_MS);
			ch_set[i].flags |= RTW_CHF_NON_OCP;
			updated = 1;
		}
	}

exit:
	return updated;
}

bool rtw_chset_update_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, -1);
}

bool rtw_chset_update_non_ocp_ms(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw,
				 u8 offset, int ms)
{
	return _rtw_chset_update_non_ocp(ch_set, ch, bw, offset, ms);
}
#endif /* CONFIG_DFS_MASTER */

int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, const u32 ch)
{
	int i;

	if (ch == 0)
		return -1;

	for (i = 0; i < MAX_CHANNEL_NUM && ch_set[i].ChannelNum != 0; i++) {
		if (ch == ch_set[i].ChannelNum)
			return i;
	}

	return -1;
}

u8 rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			   bool allow_primary_passive, bool allow_passive)
{
	u8 cch;
	u8 *op_chs;
	u8 op_ch_num;
	u8 valid = 0;
	int i;
	int ch_idx;

	cch = rtw_get_center_ch(ch, bw, offset);

	if (!rtw_get_op_chs_by_cch_bw(cch, bw, &op_chs, &op_ch_num))
		goto exit;

	for (i = 0; i < op_ch_num; i++) {
		ch_idx = rtw_chset_search_ch(ch_set, *(op_chs + i));
		if (ch_idx == -1)
			break;
		if (ch_set[ch_idx].flags & RTW_CHF_NO_IR) {
			if ((!allow_primary_passive &&
			     ch_set[ch_idx].ChannelNum == ch) ||
			    (!allow_passive &&
			     ch_set[ch_idx].ChannelNum != ch))
				break;
		}
		if (bw >= CHANNEL_WIDTH_40) {
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40U) &&
			    i % 2 == 0)
				break;
			if ((ch_set[ch_idx].flags & RTW_CHF_NO_HT40L) &&
			    i % 2 == 1)
				break;
		}
		if (bw >= CHANNEL_WIDTH_80 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_80MHZ))
			break;
		if (bw >= CHANNEL_WIDTH_160 &&
		    (ch_set[ch_idx].flags & RTW_CHF_NO_160MHZ))
			break;
	}

	if (op_ch_num != 0 && i == op_ch_num)
		valid = 1;

exit:
	return valid;
}

void rtw_chset_sync_chbw(RT_CHANNEL_INFO *ch_set, u8 *req_ch, u8 *req_bw,
			 u8 *req_offset, u8 *g_ch, u8 *g_bw, u8 *g_offset,
			 bool allow_primary_passive, bool allow_passive)
{
	u8 r_ch, r_bw, r_offset;
	u8 u_ch, u_bw, u_offset;
	u8 cur_bw = *req_bw;

	while (1) {
		r_ch = *req_ch;
		r_bw = cur_bw;
		r_offset = *req_offset;
		u_ch = *g_ch;
		u_bw = *g_bw;
		u_offset = *g_offset;

		rtw_sync_chbw(&r_ch, &r_bw, &r_offset, &u_ch, &u_bw, &u_offset);

		if (rtw_chset_is_chbw_valid(ch_set, r_ch, r_bw, r_offset,
					    allow_primary_passive,
					    allow_passive))
			break;
		if (cur_bw == CHANNEL_WIDTH_20) {
			rtw_warn_on(1);
			break;
		}
		cur_bw--;
	}

	*req_ch = r_ch;
	*req_bw = r_bw;
	*req_offset = r_offset;
	*g_ch = u_ch;
	*g_bw = u_bw;
	*g_offset = u_offset;
}

#endif /* (!CONFIG_RUST || HOST_MLME_EXT_TEST || !CONFIG_RUST_MLME_EXT_REST) && !HOST_MLME_EXT_MGNT_ATTRIB_TEST */

#if defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_MGNT_ATTRIB)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_PEER_ALIVE_TEST)))

void update_monitor_frame_attrib(_adapter *padapter, struct pkt_attrib *pattrib)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_info		*psta = NULL;
	struct sta_priv		*pstapriv = &padapter->stapriv;

	psta = rtw_get_stainfo(pstapriv, pattrib->ra);
	(void)psta;

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;
	pattrib->mac_id = RTW_DEFAULT_MGMT_MACID;
	pattrib->qsel = QSLT_MGNT;

	pattrib->pktlen = 0;

	if (pmlmeext->tx_rate == IEEE80211_CCK_RATE_1MB)
		wireless_mode = WIRELESS_11B;
	else
		wireless_mode = WIRELESS_11G;

	pattrib->raid = rtw_get_mgntframe_raid(padapter, wireless_mode);
#ifdef CONFIG_80211AC_VHT
	if (pHalData->rf_type == RF_1T1R)
		pattrib->raid = RATEID_IDX_VHT_1SS;
	else if (pHalData->rf_type == RF_2T2R || pHalData->rf_type == RF_2T4R)
		pattrib->raid = RATEID_IDX_VHT_2SS;
	else if (pHalData->rf_type == RF_3T3R)
		pattrib->raid = RATEID_IDX_VHT_3SS;
	else
		pattrib->raid = RATEID_IDX_BGN_40M_1SS;
#endif

#ifdef CONFIG_80211AC_VHT
	pattrib->rate = MGN_VHT1SS_MCS9;
#else
	pattrib->rate = MGN_MCS7;
#endif

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = 1;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

}

#ifdef CONFIG_RTW_MGMT_QUEUE
void update_mgntframe_subtype(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	struct pkt_attrib *pattrib = &pmgntframe->attrib;
	u8 *pframe;
	u8 subtype, category ,action;

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	subtype = get_frame_sub_type(pframe); /* bit(7)~bit(2) */
	pattrib->subtype = subtype;

	rtw_action_frame_parse(pframe, pattrib->pktlen, &category, &action);

	if ((subtype == WIFI_ACTION && !(action == ACT_PUBLIC_FTM_REQ || action == ACT_PUBLIC_FTM)) ||
		subtype == WIFI_DISASSOC || subtype == WIFI_DEAUTH ||
		(subtype == WIFI_PROBERSP && MLME_IS_ADHOC(padapter)))
		pattrib->ps_dontq = 0;
	else
		pattrib->ps_dontq = 1;
}
#endif

void update_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib)
{
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */

	/* _rtw_memset((u8 *)(pattrib), 0, sizeof(struct pkt_attrib)); */

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;
	pattrib->mac_id = RTW_DEFAULT_MGMT_MACID;
	pattrib->qsel = QSLT_MGNT;

#ifdef CONFIG_MCC_MODE
	update_mcc_mgntframe_attrib(padapter, pattrib);
#endif


#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, WIFI_ASOC_STATE))
#endif /* CONFIG_CONCURRENT_MODE */
		if (MLME_IS_GC(padapter)) {
			if (pwdinfo->p2p_ps_mode > P2P_PS_NONE) {
				struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
				struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
				WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);

				psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
				if (psta) {
					/* use macid sleep during NoA, mgmt frame use ac queue & ap macid */
					pattrib->mac_id = psta->cmn.mac_id;
					pattrib->qsel = QSLT_VO;
				} else {
					if (pwdinfo->p2p_ps_state != P2P_PS_DISABLE)
						RTW_ERR("%s , psta was NULL\n", __func__);
				}
			}
		}
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */

	pattrib->pktlen = 0;

	if (IS_CCK_RATE(pmlmeext->tx_rate))
		wireless_mode = WIRELESS_11B;
	else
		wireless_mode = WIRELESS_11G;
	pattrib->raid =  rtw_get_mgntframe_raid(padapter, wireless_mode);
	pattrib->rate = pmlmeext->tx_rate;

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = _FALSE;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;
#ifdef CONFIG_RTW_MGMT_QUEUE
	pattrib->ps_dontq = 1;
#endif
}

void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	u8 *pframe;
	struct pkt_attrib *pattrib = &pmgntframe->attrib;
#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE)
	struct sta_info *sta = NULL;
#endif

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;

	_rtw_memcpy(pattrib->ra, GetAddr1Ptr(pframe), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, get_addr2_ptr(pframe), ETH_ALEN);

#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE)
	sta = pattrib->psta;
	if (!sta) {
		sta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		pattrib->psta = sta;
	}

	#ifdef CONFIG_BEAMFORMING
	if (sta)
		update_attrib_txbf_info(padapter, pattrib, sta);
	#endif
#endif /* defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) || defined(CONFIG_RTW_MGMT_QUEUE) */
}

#endif /* HOST_MLME_EXT_MGNT_ATTRIB_TEST || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_MGNT_ATTRIB) && !HOST_MLME_EXT_TEST) */

#if defined(HOST_MLME_EXT_PEER_ALIVE_TEST) || \
	(((!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_EXT_PEER_ALIVE)) && \
	  !defined(HOST_MLME_EXT_TEST) && !defined(HOST_MLME_EXT_MGNT_ATTRIB_TEST)))

void rtw_delba_check(_adapter *padapter, struct sta_info *psta, u8 from_timer)
{
	int i = 0;
	int ret = _SUCCESS;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) {
		for (i = 0; i < TID_NUM; i++) {
			if ((psta->recvreorder_ctrl[i].enable) &&
			    (sta_rx_data_qos_pkts(psta, i) ==
			     sta_last_rx_data_qos_pkts(psta, i))) {
				if (_TRUE ==
				    rtw_inc_and_chk_continual_no_rx_packet(psta,
									   i)) {
					if (!from_timer)
						ret = issue_del_ba_ex(
							padapter,
							psta->cmn.mac_addr, i,
							39, 0, 3, 1);
					else
						issue_del_ba(
							padapter,
							psta->cmn.mac_addr, i,
							39, 0);
					psta->recvreorder_ctrl[i].enable =
						_FALSE;
					if (ret != _FAIL)
						psta->recvreorder_ctrl[i]
							.ampdu_size =
							RX_AMPDU_SIZE_INVALID;
					rtw_reset_continual_no_rx_packet(psta,
									 i);
				}
			} else {
				rtw_reset_continual_no_rx_packet(psta, i);
			}
		}
	}
}

u8 chk_ap_is_alive(_adapter *padapter, struct sta_info *psta)
{
	u8 ret = _FALSE;
#ifdef DBG_EXPIRATION_CHK
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_INFO(FUNC_ADPT_FMT" rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", retry:%u\n"
		 , FUNC_ADPT_ARG(padapter)
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , pmlmeext->retry
		);

	RTW_INFO(FUNC_ADPT_FMT" tx_pkts:%llu, link_count:%u\n", FUNC_ADPT_ARG(padapter)
		 , sta_tx_pkts(psta)
		 , pmlmeinfo->link_count
		);
#endif

	if ((sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta)) &&
	    sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta) &&
	    sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta))
		ret = _FALSE;
	else
		ret = _TRUE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

u8 chk_adhoc_peer_is_alive(struct sta_info *psta)
{
	u8 ret = _TRUE;

#ifdef DBG_EXPIRATION_CHK
	RTW_INFO("sta:"MAC_FMT", rssi:%d, rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", expire_to:%u\n"
		 , MAC_ARG(psta->cmn.mac_addr)
		 , psta->cmn.rssi_stat.rssi
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , psta->expire_to
		);
#endif

	if (sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta) &&
	    sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta) &&
	    sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta))
		ret = _FALSE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

#ifdef CONFIG_TDLS
u8 chk_tdls_peer_sta_is_alive(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;

	if ((psta->sta_stats.rx_data_pkts ==
	     psta->sta_stats.last_rx_data_pkts) &&
	    (psta->sta_stats.rx_tdls_disc_rsp_pkts ==
	     psta->sta_stats.last_rx_tdls_disc_rsp_pkts))
		return _FALSE;

	return _TRUE;
}
#endif /* CONFIG_TDLS */

#endif /* HOST_MLME_EXT_PEER_ALIVE_TEST || ((!CONFIG_RUST || !CONFIG_RUST_MLME_EXT_PEER_ALIVE) && !HOST_MLME_EXT_TEST) */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_EXT_MGNT_ATTRIB)
#include <drv_types.h>
#include <hal_data.h>

u8 rtw_rust_mgnt_tx_rate(_adapter *padapter)
{
	return padapter->mlmeextpriv.tx_rate;
}

u16 rtw_rust_mgnt_mgnt_seq(_adapter *padapter)
{
	return padapter->mlmeextpriv.mgnt_seq;
}

u8 rtw_rust_mgnt_hw_ssn_seq_no(_adapter *padapter)
{
	return padapter->xmitpriv.hw_ssn_seq_no;
}

u8 rtw_rust_mgnt_hal_rf_type(_adapter *padapter)
{
	return GET_HAL_DATA(padapter)->rf_type;
}

u8 rtw_rust_mgnt_mlme_is_adhoc(_adapter *padapter)
{
	return MLME_IS_ADHOC(padapter) ? 1 : 0;
}

struct sta_priv *rtw_rust_mgnt_stapriv(_adapter *padapter)
{
	return &padapter->stapriv;
}

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
u8 rtw_rust_mgnt_p2p_noa_override(_adapter *padapter, u8 *mac_id, u8 *qsel)
{
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
#ifdef CONFIG_P2P_PS
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P_PS */

#ifdef CONFIG_CONCURRENT_MODE
	if (!rtw_mi_buddy_check_fwstate(padapter, WIFI_ASOC_STATE))
		return 0;
#endif /* CONFIG_CONCURRENT_MODE */
	if (!MLME_IS_GC(padapter))
		return 0;
#ifdef CONFIG_P2P_PS
	if (pwdinfo->p2p_ps_mode <= P2P_PS_NONE)
		return 0;
#else
	return 0;
#endif /* CONFIG_P2P_PS */

	psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
	if (!psta)
		return 0;

	*mac_id = psta->cmn.mac_id;
	*qsel = QSLT_VO;
	return 1;
}
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */
#endif
