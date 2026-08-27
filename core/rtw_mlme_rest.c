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

#endif /* !CONFIG_RUST || HOST_MLME_UNASSOC_TEST || !CONFIG_RUST_MLME_UNASSOC */
#endif /* CONFIG_RTW_MULTI_AP */

#if defined(CONFIG_RUST) && !defined(HOST_MLME_TEST) && !defined(HOST_MLME_UNASSOC_TEST)
u8 *rtw_mlme_rest_bss_ies(WLAN_BSSID_EX *bss) { return bss->IEs; }
u32 *rtw_mlme_rest_bss_ssid_length(WLAN_BSSID_EX *bss) { return &bss->Ssid.SsidLength; }
u8 *rtw_mlme_rest_bss_ssid(WLAN_BSSID_EX *bss) { return bss->Ssid.Ssid; }
u8 *rtw_mlme_rest_bss_mac(WLAN_BSSID_EX *bss) { return bss->MacAddress; }
u32 *rtw_mlme_rest_network_privacy(struct wlan_network *pnetwork) { return &pnetwork->network.Privacy; }
u32 *rtw_mlme_rest_adapter_privacy(_adapter *adapter) { return &adapter->securitypriv.dot11PrivacyAlgrthm; }
#endif /* CONFIG_RUST && !HOST_MLME_TEST */
