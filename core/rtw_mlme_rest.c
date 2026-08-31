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
#define _RTW_MLME_REST_C_

#if defined(HOST_MLME_UNASSOC_TEST)
#include "host_mlme_unassoc_types.h"
#elif defined(HOST_MLME_WMM_RSN_TEST)
#include "host_mlme_wmm_rsn_types.h"
#elif defined(HOST_MLME_ROAMING_TEST)
#include "host_mlme_roaming_types.h"
#elif defined(HOST_MLME_TEST)
#include "host_mlme_types.h"
#else
#include <drv_types.h>
#endif

#if defined(HOST_MLME_TEST)

void rtw_generate_random_ibss(u8 *pibss)
{
	*((u32 *)(&pibss[2])) = rtw_random32();
	pibss[0] = 0x02; /* in ad-hoc mode local bit must set to 1 */
	pibss[1] = 0x11;
	pibss[2] = 0x87;
}

u8 *rtw_get_capability_from_ie(u8 *ie)
{
	return ie + 8 + 2;
}

u16 rtw_get_capability(WLAN_BSSID_EX *bss)
{
	u16 val;

	_rtw_memcpy((u8 *)&val, rtw_get_capability_from_ie(bss->IEs), 2);

	return le16_to_cpu(val);
}

u8 *rtw_get_timestampe_from_ie(u8 *ie)
{
	return ie + 0;
}

u8 *rtw_get_beacon_interval_from_ie(u8 *ie)
{
	return ie + 8;
}

int rtw_is_same_ibss(_adapter *adapter, struct wlan_network *pnetwork)
{
	int ret = _TRUE;
	struct security_priv *psecuritypriv = &adapter->securitypriv;

	if ((psecuritypriv->dot11PrivacyAlgrthm != _NO_PRIVACY_) &&
	    (pnetwork->network.Privacy == 0))
		ret = _FALSE;
	else if ((psecuritypriv->dot11PrivacyAlgrthm == _NO_PRIVACY_) &&
		 (pnetwork->network.Privacy == 1))
		ret = _FALSE;
	else
		ret = _TRUE;

	return ret;
}

int is_same_ess(WLAN_BSSID_EX *a, WLAN_BSSID_EX *b)
{
	return (a->Ssid.SsidLength == b->Ssid.SsidLength)
	       &&  _rtw_memcmp(a->Ssid.Ssid, b->Ssid.Ssid, a->Ssid.SsidLength) == _TRUE;
}

int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, u8 feature)
{
	u16 s_cap, d_cap;

	if (rtw_bug_check(dst, src, &s_cap, &d_cap) == _FALSE)
		return _FALSE;

	_rtw_memcpy((u8 *)&s_cap, rtw_get_capability_from_ie(src->IEs), 2);
	_rtw_memcpy((u8 *)&d_cap, rtw_get_capability_from_ie(dst->IEs), 2);

	s_cap = le16_to_cpu(s_cap);
	d_cap = le16_to_cpu(d_cap);

#ifdef CONFIG_P2P
	if ((feature == 1) &&
	    (_rtw_memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN) == _TRUE))
		return _TRUE;
#endif

	if (((_rtw_memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN)) == _TRUE) &&
	    ((s_cap & WLAN_CAPABILITY_IBSS) == (d_cap & WLAN_CAPABILITY_IBSS)) &&
	    ((s_cap & WLAN_CAPABILITY_BSS) == (d_cap & WLAN_CAPABILITY_BSS))) {
		if ((src->Ssid.SsidLength == dst->Ssid.SsidLength) &&
		    (((_rtw_memcmp(src->Ssid.Ssid, dst->Ssid.Ssid, src->Ssid.SsidLength)) == _TRUE) ||
		     (is_all_null((char *)src->Ssid.Ssid, src->Ssid.SsidLength) == _TRUE ||
		      is_all_null((char *)dst->Ssid.Ssid, dst->Ssid.SsidLength) == _TRUE)))
			return _TRUE;
		else if ((src->Ssid.SsidLength == 0 || dst->Ssid.SsidLength == 0))
			return _TRUE;
		else
			return _FALSE;
	} else {
		return _FALSE;
	}
}

#endif /* HOST_MLME_TEST */

#if defined(HOST_MLME_ROAMING_TEST)

int is_same_ess(WLAN_BSSID_EX *a, WLAN_BSSID_EX *b)
{
	return (a->Ssid.SsidLength == b->Ssid.SsidLength)
	       &&  _rtw_memcmp(a->Ssid.Ssid, b->Ssid.Ssid, a->Ssid.SsidLength) == _TRUE;
}

#endif /* HOST_MLME_ROAMING_TEST */

#ifdef CONFIG_RTW_MULTI_AP
#if !defined(CONFIG_RUST) || defined(HOST_MLME_UNASSOC_TEST) || !defined(CONFIG_RUST_MLME_UNASSOC)

static void del_unassoc_sta(struct mlme_priv *mlmepriv,
			    struct unassoc_sta_info *unassoc_sta)
{
	_irqL irqL;
	_queue *free_queue = &(mlmepriv->free_unassoc_sta_queue);

	if (unassoc_sta->interested)
		mlmepriv->interested_unassoc_sta_cnt--;
	if (mlmepriv->interested_unassoc_sta_cnt == 0) {
		rtw_run_in_thread_cmd(mlme_to_adapter(mlmepriv),
				      ((void *)(rtw_hal_rcr_set_chk_bssid_act_non)),
				      mlme_to_adapter(mlmepriv));
	}

	_enter_critical_bh(&free_queue->lock, &irqL);
	rtw_list_delete(&(unassoc_sta->list));
	rtw_list_insert_tail(&(unassoc_sta->list), &(free_queue->queue));
	_exit_critical_bh(&free_queue->lock, &irqL);
}

static u8 del_unassoc_sta_chk(struct mlme_priv *mlmepriv,
			      struct unassoc_sta_info *unassoc_sta)
{
	systime cur, lifetime;

	if (unassoc_sta == NULL)
		return UNASOC_STA_DEL_CHK_SKIP;

	if (unassoc_sta->interested)
		return UNASOC_STA_DEL_CHK_SKIP;

	cur = rtw_get_current_time();
	lifetime = unassoc_sta->time + rtw_ms_to_systime(UNASSOC_STA_LIFETIME_MS);
	if (rtw_time_before(cur, lifetime))
		return UNASOC_STA_DEL_CHK_ALIVE;

	del_unassoc_sta(mlmepriv, unassoc_sta);

	return UNASOC_STA_DEL_CHK_DELETED;
}

static struct unassoc_sta_info *alloc_unassoc_sta(struct mlme_priv *mlmepriv)
{
	_irqL irqL;
	struct unassoc_sta_info *unassoc_sta;
	_queue *free_queue = &mlmepriv->free_unassoc_sta_queue;
	_list *list = NULL;

	_enter_critical_bh(&free_queue->lock, &irqL);

	if (_rtw_queue_empty(free_queue) == _TRUE) {
		unassoc_sta = NULL;
		goto exit;
	}
	list = get_next(&(free_queue->queue));

	unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);

	rtw_list_delete(&unassoc_sta->list);

	_rtw_memset(unassoc_sta->addr, 0, ETH_ALEN);
	unassoc_sta->recv_signal_power = 0;
	unassoc_sta->time = 0;
	unassoc_sta->interested = 0;
exit:
	_exit_critical_bh(&free_queue->lock, &irqL);

	return unassoc_sta;
}

void rtw_rx_add_unassoc_sta(_adapter *adapter, u8 stype, u8 *addr,
			    s8 recv_signal_power)
{
	struct unassoc_sta_info *unassoc_sta;
	struct unassoc_sta_info *oldest_unassoc_sta = NULL;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		if (_rtw_memcmp(addr, unassoc_sta->addr, ETH_ALEN) == _TRUE) {
			if (unassoc_sta->interested
			    || mlmepriv->unassoc_sta_mode_of_stype[stype]
			       >= UNASOC_STA_MODE_ALL) {
				unassoc_sta->recv_signal_power = recv_signal_power;
				unassoc_sta->time = rtw_get_current_time();
				goto unlock_unassoc_sta_queue;
			}
		}

		if (del_unassoc_sta_chk(mlmepriv, unassoc_sta)
		    == UNASOC_STA_DEL_CHK_ALIVE) {
			if (oldest_unassoc_sta == NULL)
				oldest_unassoc_sta = unassoc_sta;
			else if (rtw_time_before(unassoc_sta->time,
						 oldest_unassoc_sta->time))
				oldest_unassoc_sta = unassoc_sta;
		}
	}

	if (mlmepriv->unassoc_sta_mode_of_stype[stype]
	    <= UNASOC_STA_MODE_INTERESTED)
		goto unlock_unassoc_sta_queue;

	unassoc_sta = alloc_unassoc_sta(mlmepriv);
	if (unassoc_sta == NULL) {
		if (oldest_unassoc_sta) {
			del_unassoc_sta(mlmepriv, oldest_unassoc_sta);
			unassoc_sta = alloc_unassoc_sta(mlmepriv);
		} else {
			goto unlock_unassoc_sta_queue;
		}
	}
	_rtw_memcpy(unassoc_sta->addr, addr, ETH_ALEN);
	unassoc_sta->recv_signal_power = recv_signal_power;
	unassoc_sta->time = rtw_get_current_time();
	rtw_list_insert_tail(&(unassoc_sta->list), &(queue->queue));

unlock_unassoc_sta_queue:
	_exit_critical_bh(&queue->lock, &irqL);
}

void rtw_del_unassoc_sta_queue(_adapter *adapter)
{
	struct unassoc_sta_info *unassoc_sta;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		del_unassoc_sta(mlmepriv, unassoc_sta);
	}

	_exit_critical_bh(&queue->lock, &irqL);
}

void rtw_del_unassoc_sta(_adapter *adapter, u8 *addr)
{
	struct unassoc_sta_info *unassoc_sta;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		if (_rtw_memcmp(addr, unassoc_sta->addr, ETH_ALEN) == _TRUE) {
			del_unassoc_sta(mlmepriv, unassoc_sta);
			goto unlock_unassoc_sta_queue;
		}
	}

unlock_unassoc_sta_queue:
	_exit_critical_bh(&queue->lock, &irqL);
}

u8 rtw_search_unassoc_sta(_adapter *adapter, u8 *addr,
			  struct unassoc_sta_info *ret_sta)
{
	struct unassoc_sta_info *unassoc_sta = NULL;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;
	u8 searched = 0;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		if (_rtw_memcmp(addr, unassoc_sta->addr, ETH_ALEN) == _TRUE) {
			memcpy(ret_sta, unassoc_sta, sizeof(struct unassoc_sta_info));
			searched = 1;
			break;
		}
	}
	_exit_critical_bh(&queue->lock, &irqL);

	return searched;
}

void rtw_add_interested_unassoc_sta(_adapter *adapter, u8 *addr)
{
	struct unassoc_sta_info *unassoc_sta;
	struct unassoc_sta_info *oldest_unassoc_sta = NULL;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		if (_rtw_memcmp(addr, unassoc_sta->addr, ETH_ALEN) == _TRUE) {
			if (!unassoc_sta->interested) {
				unassoc_sta->interested = 1;
				mlmepriv->interested_unassoc_sta_cnt++;
				if (mlmepriv->interested_unassoc_sta_cnt == 1) {
					rtw_run_in_thread_cmd(mlme_to_adapter(mlmepriv),
							      ((void *)(rtw_hal_rcr_set_chk_bssid_act_non)),
							      mlme_to_adapter(mlmepriv));
				}
			}
			goto unlock_unassoc_sta_queue;
		}

		if (del_unassoc_sta_chk(mlmepriv, unassoc_sta)
		    == UNASOC_STA_DEL_CHK_ALIVE) {
			if (oldest_unassoc_sta == NULL)
				oldest_unassoc_sta = unassoc_sta;
			else if (rtw_time_after(unassoc_sta->time,
						oldest_unassoc_sta->time))
				oldest_unassoc_sta = unassoc_sta;
		}
	}
	unassoc_sta = alloc_unassoc_sta(mlmepriv);
	if (unassoc_sta == NULL) {
		RTW_INFO(FUNC_ADPT_FMT": Allocate fail\n", FUNC_ADPT_ARG(adapter));
		if (oldest_unassoc_sta) {
			RTW_INFO(FUNC_ADPT_FMT": Delete oldest entry and try again.\n",
				 FUNC_ADPT_ARG(adapter));
			del_unassoc_sta(mlmepriv, oldest_unassoc_sta);
			unassoc_sta = alloc_unassoc_sta(mlmepriv);
		} else {
			goto unlock_unassoc_sta_queue;
		}
	}
	_rtw_memcpy(unassoc_sta->addr, addr, ETH_ALEN);
	unassoc_sta->interested = 1;
	unassoc_sta->recv_signal_power = 0;
	unassoc_sta->time = rtw_get_current_time() -
			    rtw_ms_to_systime(UNASSOC_STA_LIFETIME_MS);
	rtw_list_insert_tail(&(unassoc_sta->list), &(queue->queue));
	mlmepriv->interested_unassoc_sta_cnt++;
	if (mlmepriv->interested_unassoc_sta_cnt == 1) {
		rtw_run_in_thread_cmd(mlme_to_adapter(mlmepriv),
				      ((void *)(rtw_hal_rcr_set_chk_bssid_act_non)),
				      mlme_to_adapter(mlmepriv));
	}

unlock_unassoc_sta_queue:
	_exit_critical_bh(&queue->lock, &irqL);
}

void rtw_undo_interested_unassoc_sta(_adapter *adapter, u8 *addr)
{
	struct unassoc_sta_info *unassoc_sta;
	struct mlme_priv *mlmepriv;
	_queue *queue;
	_irqL irqL;
	_list *head, *list;

	adapter = GET_PRIMARY_ADAPTER(adapter);
	mlmepriv = &(adapter->mlmepriv);
	queue = &(mlmepriv->unassoc_sta_queue);

	_enter_critical_bh(&queue->lock, &irqL);
	head = get_list_head(queue);
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		unassoc_sta = LIST_CONTAINOR(list, struct unassoc_sta_info, list);
		list = get_next(list);

		if (_rtw_memcmp(addr, unassoc_sta->addr, ETH_ALEN) == _TRUE) {
			if (unassoc_sta->interested) {
				unassoc_sta->interested = 0;
				mlmepriv->interested_unassoc_sta_cnt--;
				if (mlmepriv->interested_unassoc_sta_cnt == 0) {
					rtw_run_in_thread_cmd(mlme_to_adapter(mlmepriv),
							      ((void *)(rtw_hal_rcr_set_chk_bssid_act_non)),
							      mlme_to_adapter(mlmepriv));
				}
			}
			goto unlock_unassoc_sta_queue;
		}
	}
unlock_unassoc_sta_queue:
	_exit_critical_bh(&queue->lock, &irqL);
}

#endif /* !CONFIG_RUST || HOST_MLME_UNASSOC_TEST || !CONFIG_RUST_MLME_UNASSOC */
#endif /* CONFIG_RTW_MULTI_AP */

#ifdef CONFIG_LAYER2_ROAMING
#if !defined(CONFIG_RUST) || defined(HOST_MLME_ROAMING_TEST) || !defined(CONFIG_RUST_MLME_ROAMING)

extern int rtw_is_desired_network(_adapter *adapter, struct wlan_network *pnetwork);

/*
 * Select a new roaming candidate from the original @param candidate and @param competitor
 * @return _TRUE: candidate is updated
 * @return _FALSE: candidate is not updated
 */
int rtw_check_roaming_candidate(struct mlme_priv *mlme
	, struct wlan_network **candidate, struct wlan_network *competitor)
{
	int updated = _FALSE;
	_adapter *adapter = container_of(mlme, _adapter, mlmepriv);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	RT_CHANNEL_INFO *chset = rfctl->channel_set;
	u8 ch = competitor->network.Configuration.DSConfig;

	if (rtw_chset_search_ch(chset, ch) < 0)
		goto exit;
	if (IS_DFS_SLAVE_WITH_RD(rfctl)
		&& !rtw_rfctl_dfs_domain_unknown(rfctl)
		&& rtw_chset_is_ch_non_ocp(chset, ch))
		goto exit;

#if defined(CONFIG_RTW_REPEATER_SON) &&  (!defined(CONFIG_RTW_REPEATER_SON_ROOT))
	if (rtw_rson_isupdate_roamcan(mlme, candidate, competitor))
		goto  update;
	goto exit;
#endif

	if (is_same_ess(&competitor->network, &mlme->cur_network.network) == _FALSE)
		goto exit;

	if (rtw_is_desired_network(adapter, competitor) == _FALSE)
		goto exit;

#ifdef CONFIG_RTW_ROAM_QUICKSCAN
	if (competitor->network.PhyInfo.SignalStrength > CONFIG_RTW_ROAM_QUICKSCAN_TH)
		adapter->mlmeextpriv.quickscan_next = _TRUE;
#endif

#ifdef CONFIG_LAYER2_ROAMING
	if (mlme->need_to_roam == _FALSE)
		goto exit;
#endif

	RTW_INFO("roam candidate:%s %s("MAC_FMT", ch%3u) rssi:%d dBm, age:%5d\n",
		 (competitor == mlme->cur_network_scanned) ? "*" : " " ,
		 competitor->network.Ssid.Ssid,
		 MAC_ARG(competitor->network.MacAddress),
		 competitor->network.Configuration.DSConfig,
		 (int)competitor->network.Rssi,
		 rtw_get_passing_time_ms(competitor->last_scanned)
		);

	/* got specific addr to roam */
	if (!is_zero_mac_addr(mlme->roam_tgt_addr)) {
		if (_rtw_memcmp(mlme->roam_tgt_addr, competitor->network.MacAddress, ETH_ALEN) == _TRUE)
			goto update;
		else
			goto exit;
	}

#ifdef CONFIG_RTW_80211R
	if (rtw_ft_chk_flags(adapter, RTW_FT_PEER_EN)) {
		if (rtw_ft_chk_roaming_candidate(adapter, competitor) == _FALSE)
		goto exit;
	}

#ifdef CONFIG_RTW_WNM
	if (rtw_wnm_btm_diff_bss(adapter) &&
		rtw_wnm_btm_roam_candidate(adapter, competitor)) {
		goto update;
	}
#endif
#endif

#if 1
	if (rtw_get_passing_time_ms(competitor->last_scanned) >= mlme->roam_scanr_exp_ms)
		goto exit;

	if (competitor->network.Rssi - mlme->cur_network_scanned->network.Rssi < mlme->roam_rssi_diff_th)
		goto exit;

	if (*candidate != NULL && (*candidate)->network.Rssi >= competitor->network.Rssi)
		goto exit;
#else
	goto exit;
#endif

update:
	*candidate = competitor;
	updated = _TRUE;

exit:
	return updated;
}

int rtw_select_roaming_candidate(struct mlme_priv *mlme)
{
	_irqL	irqL;
	int ret = _FAIL;
	_list	*phead;
	_adapter *adapter __attribute__((unused));
	_queue	*queue	= &(mlme->scanned_queue);
	struct	wlan_network	*pnetwork = NULL;
	struct	wlan_network	*candidate = NULL;

	if (mlme->cur_network_scanned == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	_enter_critical_bh(&(mlme->scanned_queue.lock), &irqL);
	phead = get_list_head(queue);
	adapter = (_adapter *)mlme->nic_hdl;
	(void)adapter;

	mlme->pscanned = get_next(phead);

	while (!rtw_end_of_queue_search(phead, mlme->pscanned)) {

		pnetwork = LIST_CONTAINOR(mlme->pscanned, struct wlan_network, list);
		if (pnetwork == NULL) {
			ret = _FAIL;
			goto exit;
		}

		mlme->pscanned = get_next(mlme->pscanned);

		if (0)
			RTW_INFO("%s("MAC_FMT", ch%u) rssi:%d\n"
				 , pnetwork->network.Ssid.Ssid
				 , MAC_ARG(pnetwork->network.MacAddress)
				 , pnetwork->network.Configuration.DSConfig
				 , (int)pnetwork->network.Rssi);

		rtw_check_roaming_candidate(mlme, &candidate, pnetwork);

	}

	if (candidate == NULL) {
	/*	if parent note lost the path to root and there is no other cadidate, report disconnection	*/
#if defined(CONFIG_RTW_REPEATER_SON) &&  (!defined(CONFIG_RTW_REPEATER_SON_ROOT))
		struct rtw_rson_struct  rson_curr;
		u8 rson_score;

		rtw_get_rson_struct(&(mlme->cur_network_scanned->network), &rson_curr);
		rson_score = rtw_cal_rson_score(&rson_curr, mlme->cur_network_scanned->network.Rssi);
		if (check_fwstate(mlme, WIFI_ASOC_STATE)
			&& ((rson_score == RTW_RSON_SCORE_NOTCNNT)
			|| (rson_score == RTW_RSON_SCORE_NOTSUP)))
			receive_disconnect(adapter, mlme->cur_network_scanned->network.MacAddress
								, WLAN_REASON_EXPIRATION_CHK, _FALSE);
#endif
		RTW_INFO("%s: return _FAIL(candidate == NULL)\n", __FUNCTION__);
		ret = _FAIL;
		goto exit;
	} else {
#if defined(CONFIG_RTW_REPEATER_SON) &&  (!defined(CONFIG_RTW_REPEATER_SON_ROOT))
		struct rtw_rson_struct  rson_curr;
		u8 rson_score;

		rtw_get_rson_struct(&(candidate->network), &rson_curr);
		rson_score = rtw_cal_rson_score(&rson_curr, candidate->network.Rssi);
		RTW_INFO("%s: candidate: %s("MAC_FMT", ch:%u) rson_score:%d\n", __FUNCTION__,
			candidate->network.Ssid.Ssid, MAC_ARG(candidate->network.MacAddress),
			 candidate->network.Configuration.DSConfig, rson_score);
#else
		RTW_INFO("%s: candidate: %s("MAC_FMT", ch:%u)\n", __FUNCTION__,
			candidate->network.Ssid.Ssid, MAC_ARG(candidate->network.MacAddress),
			 candidate->network.Configuration.DSConfig);
#endif
		mlme->roam_network = candidate;

		if (_rtw_memcmp(candidate->network.MacAddress, mlme->roam_tgt_addr, ETH_ALEN) == _TRUE)
			_rtw_memset(mlme->roam_tgt_addr, 0, ETH_ALEN);
	}

	ret = _SUCCESS;
exit:
	_exit_critical_bh(&(mlme->scanned_queue.lock), &irqL);

	return ret;
}

#endif /* !CONFIG_RUST || HOST_MLME_ROAMING_TEST || !CONFIG_RUST_MLME_ROAMING */
#endif /* CONFIG_LAYER2_ROAMING */

#if (defined(HOST_MLME_WMM_RSN_TEST) && !defined(RUST_MLME_WMM_RSN_ORACLE)) || \
     (!defined(HOST_MLME_TEST) && !defined(HOST_MLME_UNASSOC_TEST) && \
      !defined(HOST_MLME_ROAMING_TEST) && \
      (!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_WMM_RSN)))

/* adjust IEs for rtw_joinbss_cmd in WMM */
int rtw_restruct_wmm_ie(_adapter *adapter, u8 *in_ie, u8 *out_ie, uint in_len, uint initial_out_len)
{
#ifdef CONFIG_WMMPS_STA
	struct mlme_priv		*pmlmepriv = &adapter->mlmepriv;
	struct qos_priv		*pqospriv = &pmlmepriv->qospriv;
#endif /* CONFIG_WMMPS_STA */
	unsigned	int ielength = 0;
	unsigned int i, j;
	u8 qos_info = 0;

	i = 12; /* after the fixed IE */
	while (i < in_len) {
		ielength = initial_out_len;

		if (in_ie[i] == 0xDD && in_ie[i + 2] == 0x00 && in_ie[i + 3] == 0x50  && in_ie[i + 4] == 0xF2 && in_ie[i + 5] == 0x02 && i + 5 < in_len) { /* WMM element ID and OUI */

			for (j = i; j < i + 9; j++) {
				out_ie[ielength] = in_ie[j];
				ielength++;
			}
			out_ie[initial_out_len + 1] = 0x07;
			out_ie[initial_out_len + 6] = 0x00;

#ifdef CONFIG_WMMPS_STA
			switch(pqospriv->uapsd_max_sp_len) {
				case NO_LIMIT:
					break;
				case TWO_MSDU:
					SET_FLAG(qos_info, BIT5);
					break;
				case FOUR_MSDU:
					SET_FLAG(qos_info, BIT6);
					break;
				case SIX_MSDU:
					SET_FLAG(qos_info, BIT5);
					SET_FLAG(qos_info, BIT6);
					break;
				default:
					break;
			};

			if((TEST_FLAG(pqospriv->uapsd_tid, WMM_TID7)) && (TEST_FLAG(pqospriv->uapsd_tid, WMM_TID6)))
				SET_FLAG(qos_info, WMM_IE_UAPSD_VO);
			if((TEST_FLAG(pqospriv->uapsd_tid, WMM_TID5)) && (TEST_FLAG(pqospriv->uapsd_tid, WMM_TID4)))
				SET_FLAG(qos_info, WMM_IE_UAPSD_VI);
			if((TEST_FLAG(pqospriv->uapsd_tid, WMM_TID2)) && (TEST_FLAG(pqospriv->uapsd_tid, WMM_TID1)))
				SET_FLAG(qos_info, WMM_IE_UAPSD_BK);
			if((TEST_FLAG(pqospriv->uapsd_tid, WMM_TID3)) && (TEST_FLAG(pqospriv->uapsd_tid, WMM_TID0)))
				SET_FLAG(qos_info, WMM_IE_UAPSD_BE);
#endif /* CONFIG_WMMPS_STA */

			out_ie[initial_out_len + 8] = qos_info;

			break;
		}

		i += (in_ie[i + 1] + 2); /* to the next IE element */
	}

	return ielength;

}

#endif /* HOST_MLME_WMM_RSN_TEST || (kernel && (!CONFIG_RUST || !CONFIG_RUST_MLME_WMM_RSN)) */

#if defined(HOST_MLME_WMM_RSN_TEST) || \
     (!defined(HOST_MLME_TEST) && !defined(HOST_MLME_UNASSOC_TEST) && \
      !defined(HOST_MLME_ROAMING_TEST))

#if !defined(CONFIG_RUST) || defined(HOST_MLME_WMM_RSN_TEST) || !defined(CONFIG_RUST_MLME_WMM_RSN)

static int SecIsInPMKIDList(_adapter *Adapter, u8 *bssid)
{
	struct security_priv *psecuritypriv = &Adapter->securitypriv;
	int i = 0;

	do {
		if ((psecuritypriv->PMKIDList[i].bUsed) &&
		    (_rtw_memcmp(psecuritypriv->PMKIDList[i].Bssid, bssid, ETH_ALEN) == _TRUE))
			break;
		else
			i++;
	} while (i < NUM_PMKID_CACHE);

	return (i == NUM_PMKID_CACHE) ? -1 : i;
}

int rtw_cached_pmkid(_adapter *Adapter, u8 *bssid)
{
	return SecIsInPMKIDList(Adapter, bssid);
}

int rtw_rsn_sync_pmkid(_adapter *adapter, u8 *ie, uint ie_len, int i_ent)
{
	struct security_priv *sec = &adapter->securitypriv;
	struct rsne_info info;
	u8 gm_cs[4] = {0};
	int i;

	rtw_rsne_info_parse(ie, ie_len, &info);
	if (info.err) {
		RTW_WARN(FUNC_ADPT_FMT" rtw_rsne_info_parse error\n", FUNC_ADPT_ARG(adapter));
		return 0;
	}
	if (i_ent < 0 && info.pmkid_cnt == 0)
		goto exit;
	if (info.pmkid_list == NULL)
		goto exit;
	if (i_ent >= 0 && info.pmkid_cnt == 1 &&
	    _rtw_memcmp(info.pmkid_list, sec->PMKIDList[i_ent].PMKID, 16))
		goto exit;
	if (info.gmcs)
		_rtw_memcpy(gm_cs, info.gmcs, 4);
	if (info.pmkid_cnt) {
		for (i = 0; i < info.pmkid_cnt; i++)
			RTW_INFO("    "KEY_FMT"\n", KEY_ARG(info.pmkid_list + i * 16));
	}
	if (i_ent >= 0) {
		info.pmkid_cnt = 1;
		_rtw_memcpy(info.pmkid_list, sec->PMKIDList[i_ent].PMKID, 16);
	} else
		info.pmkid_cnt = 0;
	RTW_PUT_LE16(info.pmkid_list - 2, info.pmkid_cnt);
	if (info.gmcs)
		_rtw_memcpy(info.pmkid_list + 16 * info.pmkid_cnt, gm_cs, 4);
	ie_len = 1 + 1 + 2 + 4 + 2 + 4 * info.pcs_cnt + 2 + 4 * info.akm_cnt + 2
		+ 2 + 16 * info.pmkid_cnt + (info.gmcs ? 4 : 0);
	ie[1] = (u8)(ie_len - 2);
exit:
	return ie_len;
}

#endif /* !CONFIG_RUST || HOST_MLME_WMM_RSN_TEST || !CONFIG_RUST_MLME_WMM_RSN */

#if (defined(HOST_MLME_WMM_RSN_TEST) && !defined(RUST_MLME_WMM_RSN_ORACLE)) || \
     (!defined(HOST_MLME_TEST) && !defined(HOST_MLME_UNASSOC_TEST) && \
      !defined(HOST_MLME_ROAMING_TEST) && \
      (!defined(CONFIG_RUST) || !defined(CONFIG_RUST_MLME_WMM_RSN)))

sint rtw_restruct_sec_ie(_adapter *adapter, u8 *out_ie)
{
	u8 authmode = 0x0;
	uint ielength = 0;
	int iEntry;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct security_priv *psecuritypriv = &adapter->securitypriv;
	uint ndisauthmode = psecuritypriv->ndisauthtype;

	if ((ndisauthmode == Ndis802_11AuthModeWPA) ||
	    (ndisauthmode == Ndis802_11AuthModeWPAPSK))
		authmode = _WPA_IE_ID_;
	if ((ndisauthmode == Ndis802_11AuthModeWPA2) ||
	    (ndisauthmode == Ndis802_11AuthModeWPA2PSK))
		authmode = _WPA2_IE_ID_;

	if (check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		_rtw_memcpy(out_ie, psecuritypriv->wps_ie,
			    psecuritypriv->wps_ie_len);
		ielength = psecuritypriv->wps_ie_len;
	} else if ((authmode == _WPA_IE_ID_) || (authmode == _WPA2_IE_ID_)) {
		_rtw_memcpy(out_ie, psecuritypriv->supplicant_ie,
			    psecuritypriv->supplicant_ie[1] + 2);
		ielength = psecuritypriv->supplicant_ie[1] + 2;
		rtw_report_sec_ie(adapter, authmode,
				  psecuritypriv->supplicant_ie);
	}

	if (authmode == WLAN_EID_RSN) {
#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_WMM_RSN)
		iEntry = rtw_cached_pmkid(adapter, pmlmepriv->assoc_bssid);
#else
		iEntry = SecIsInPMKIDList(adapter, pmlmepriv->assoc_bssid);
#endif
		ielength = rtw_rsn_sync_pmkid(adapter, out_ie, ielength, iEntry);
	}

	if ((psecuritypriv->auth_type == MLME_AUTHTYPE_SAE) &&
	    (psecuritypriv->rsnx_ie_len >= 3)) {
		u8 *_pos = out_ie + (psecuritypriv->supplicant_ie[1] + 2);

		_rtw_memcpy(_pos, psecuritypriv->rsnx_ie,
			    psecuritypriv->rsnx_ie_len);
		ielength += psecuritypriv->rsnx_ie_len;
		RTW_INFO_DUMP("update IE for RSNX :", out_ie, ielength);
	}

	return ielength;
}

#endif /* HOST_MLME_WMM_RSN_TEST w/ C oracle || kernel w/o Rust WMM/RSN */

#endif /* HOST_MLME_WMM_RSN_TEST || (kernel && !HOST_MLME_TEST && !HOST_MLME_UNASSOC && !HOST_MLME_ROAMING) */

#if defined(CONFIG_RUST) && !defined(HOST_MLME_TEST) && !defined(HOST_MLME_UNASSOC_TEST) && \
    !defined(HOST_MLME_WMM_RSN_TEST) && !defined(HOST_MLME_ROAMING_TEST)
u8 *rtw_mlme_rest_bss_ies(WLAN_BSSID_EX *bss) { return bss->IEs; }
u32 *rtw_mlme_rest_bss_ssid_length(WLAN_BSSID_EX *bss) { return &bss->Ssid.SsidLength; }
u8 *rtw_mlme_rest_bss_ssid(WLAN_BSSID_EX *bss) { return bss->Ssid.Ssid; }
u8 *rtw_mlme_rest_bss_mac(WLAN_BSSID_EX *bss) { return bss->MacAddress; }
u32 *rtw_mlme_rest_network_privacy(struct wlan_network *pnetwork) { return &pnetwork->network.Privacy; }
u32 *rtw_mlme_rest_adapter_privacy(_adapter *adapter) { return &adapter->securitypriv.dot11PrivacyAlgrthm; }
#endif /* CONFIG_RUST && !HOST_MLME_TEST */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_WMM_RSN) && \
    !defined(HOST_MLME_WMM_RSN_TEST)
void rtw_mlme_wmm_rsn_qos_fields(_adapter *a, u8 *max_sp, u16 *tid)
{
	*max_sp = a->mlmepriv.qospriv.uapsd_max_sp_len;
	*tid = a->mlmepriv.qospriv.uapsd_tid;
}
RT_PMKID_LIST *rtw_mlme_wmm_rsn_pmkid(_adapter *a, int i)
{
	return &a->securitypriv.PMKIDList[i];
}

u32 rtw_mlme_wmm_rsn_ndisauthtype(_adapter *a)
{
	return a->securitypriv.ndisauthtype;
}

u32 rtw_mlme_wmm_rsn_fw_state(_adapter *a)
{
	return a->mlmepriv.fw_state;
}

u8 *rtw_mlme_wmm_rsn_assoc_bssid(_adapter *a)
{
	return a->mlmepriv.assoc_bssid;
}

u8 *rtw_mlme_wmm_rsn_wps_ie(_adapter *a)
{
	return a->securitypriv.wps_ie;
}

int rtw_mlme_wmm_rsn_wps_ie_len(_adapter *a)
{
	return a->securitypriv.wps_ie_len;
}

u8 *rtw_mlme_wmm_rsn_supplicant_ie(_adapter *a)
{
	return a->securitypriv.supplicant_ie;
}

u8 rtw_mlme_wmm_rsn_auth_type(_adapter *a)
{
	return a->securitypriv.auth_type;
}

u8 *rtw_mlme_wmm_rsn_rsnx_ie(_adapter *a)
{
	return a->securitypriv.rsnx_ie;
}

int rtw_mlme_wmm_rsn_rsnx_ie_len(_adapter *a)
{
	return a->securitypriv.rsnx_ie_len;
}

void rtw_mlme_wmm_rsn_report_sec_ie(_adapter *a, u8 authmode, u8 *sec_ie)
{
	rtw_report_sec_ie(a, authmode, sec_ie);
}
#endif

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_UNASSOC) && \
    defined(CONFIG_RTW_MULTI_AP) && !defined(HOST_MLME_UNASSOC_TEST)
struct mlme_priv *rtw_rust_mlme_unassoc_adapter_mlme(_adapter *adapter)
{
	return &adapter->mlmepriv;
}

_adapter *rtw_rust_mlme_unassoc_primary(_adapter *adapter)
{
	return GET_PRIMARY_ADAPTER(adapter);
}
#endif

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_MLME_ROAMING) && \
    defined(CONFIG_LAYER2_ROAMING) && !defined(HOST_MLME_ROAMING_TEST)

_adapter *rtw_rust_mlme_roaming_adapter(struct mlme_priv *mlme)
{
	return container_of(mlme, _adapter, mlmepriv);
}

RT_CHANNEL_INFO *rtw_rust_mlme_roaming_chset(_adapter *adapter)
{
	return adapter_to_rfctl(adapter)->channel_set;
}

struct wlan_network *rtw_rust_mlme_roaming_cur_scanned(struct mlme_priv *mlme)
{
	return mlme->cur_network_scanned;
}

WLAN_BSSID_EX *rtw_rust_mlme_roaming_cur_network(struct mlme_priv *mlme)
{
	return &mlme->cur_network.network;
}

int rtw_rust_mlme_roaming_need_to_roam(struct mlme_priv *mlme)
{
	return mlme->need_to_roam ? _TRUE : _FALSE;
}

u8 *rtw_rust_mlme_roaming_roam_tgt_addr(struct mlme_priv *mlme)
{
	return mlme->roam_tgt_addr;
}

u32 rtw_rust_mlme_roaming_scanr_exp_ms(struct mlme_priv *mlme)
{
	return mlme->roam_scanr_exp_ms;
}

s32 rtw_rust_mlme_roaming_rssi_diff_th(struct mlme_priv *mlme)
{
	return (s32)mlme->roam_rssi_diff_th;
}

_list *rtw_rust_mlme_roaming_scanned_head(struct mlme_priv *mlme)
{
	return get_list_head(&mlme->scanned_queue);
}

struct wlan_network **rtw_rust_mlme_roaming_roam_network_ptr(struct mlme_priv *mlme)
{
	return &mlme->roam_network;
}

_list **rtw_rust_mlme_roaming_pscanned_ptr(struct mlme_priv *mlme)
{
	return &mlme->pscanned;
}

void rtw_rust_mlme_roaming_enter_scanned(struct mlme_priv *mlme, _irqL *irq)
{
	_enter_critical_bh(&mlme->scanned_queue.lock, irq);
}

void rtw_rust_mlme_roaming_exit_scanned(struct mlme_priv *mlme, _irqL *irq)
{
	_exit_critical_bh(&mlme->scanned_queue.lock, irq);
}

u32 rtw_rust_mlme_roaming_net_dsconfig(struct wlan_network *net)
{
	return net->network.Configuration.DSConfig;
}

NDIS_802_11_RSSI rtw_rust_mlme_roaming_net_rssi(struct wlan_network *net)
{
	return net->network.Rssi;
}

u8 *rtw_rust_mlme_roaming_net_mac(struct wlan_network *net)
{
	return net->network.MacAddress;
}

WLAN_BSSID_EX *rtw_rust_mlme_roaming_net_bss(struct wlan_network *net)
{
	return &net->network;
}

systime rtw_rust_mlme_roaming_net_last_scanned(struct wlan_network *net)
{
	return net->last_scanned;
}
#endif
