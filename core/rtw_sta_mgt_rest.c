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
#define _RTW_STA_MGT_REST_C_

#ifdef HOST_STA_MGT_TEST
#include "host_sta_mgt_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST)

bool test_st_match_rule(_adapter *adapter, u8 *local_naddr, u8 *local_port,
			u8 *remote_naddr, u8 *remote_port)
{
	if (ntohs(*((u16 *)local_port)) == 5001 ||
	    ntohs(*((u16 *)remote_port)) == 5001)
		return _TRUE;
	return _FALSE;
}

#if CONFIG_RTW_MACADDR_ACL

u8 _rtw_access_ctrl(_adapter *adapter, u8 period, const u8 *mac_addr)
{
	u8 res = _TRUE;
	_irqL irqL;
	_list *list, *head;
	struct rtw_wlan_acl_node *acl_node;
	u8 match = _FALSE;
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue *acl_node_q;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		goto exit;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	if (acl->mode != RTW_ACL_MODE_ACCEPT_UNLESS_LISTED &&
	    acl->mode != RTW_ACL_MODE_DENY_UNLESS_LISTED)
		goto exit;

	_enter_critical_bh(&(acl_node_q->lock), &irqL);
	head = get_list_head(acl_node_q);
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (_rtw_memcmp(acl_node->addr, mac_addr, ETH_ALEN)) {
			if (acl_node->valid == _TRUE) {
				match = _TRUE;
				break;
			}
		}
	}
	_exit_critical_bh(&(acl_node_q->lock), &irqL);

	if (acl->mode == RTW_ACL_MODE_ACCEPT_UNLESS_LISTED)
		res = (match == _TRUE) ? _FALSE : _TRUE;
	else /* RTW_ACL_MODE_DENY_UNLESS_LISTED */
		res = (match == _TRUE) ? _TRUE : _FALSE;

exit:
	return res;
}

u8 rtw_access_ctrl(_adapter *adapter, const u8 *mac_addr)
{
	int i;

	for (i = 0; i < RTW_ACL_PERIOD_NUM; i++)
		if (_rtw_access_ctrl(adapter, i, mac_addr) == _FALSE)
			return _FALSE;

	return _TRUE;
}

#endif /* CONFIG_RTW_MACADDR_ACL */

#endif /* !CONFIG_RUST || HOST_STA_MGT_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST)

#ifdef CONFIG_AP_MODE
u16 rtw_aid_alloc(_adapter *adapter, struct sta_info *sta)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u16 aid, i, used_cnt = 0;

	aid = 0;
	for (i = 0; i < stapriv->max_aid; i++) {
		aid = ((i + stapriv->started_aid - 1) % stapriv->max_aid) + 1;
		if (stapriv->sta_aid[aid - 1] == NULL)
			break;
		if (++used_cnt >= stapriv->max_num_sta)
			break;
	}

	/* check for aid limit and assoc limit  */
	if (i >= stapriv->max_aid || used_cnt >= stapriv->max_num_sta)
		aid = 0;

	sta->cmn.aid = aid;
	if (aid) {
		stapriv->sta_aid[aid - 1] = sta;
		if (stapriv->rr_aid)
			stapriv->started_aid = (aid % stapriv->max_aid) + 1;
	}

	return aid;
}
#endif /* CONFIG_AP_MODE */

bool rtw_is_pre_link_sta(struct sta_priv *stapriv, u8 *addr)
{
#if CONFIG_RTW_PRE_LINK_STA
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	u8 exist = _FALSE;
	int i;
	_irqL irqL;

	_enter_critical_bh(&(pre_link_sta_ctl->lock), &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, addr, ETH_ALEN) == _TRUE
		) {
			exist = _TRUE;
			break;
		}
	}
	_exit_critical_bh(&(pre_link_sta_ctl->lock), &irqL);

	return exist;
#else
	return _FALSE;
#endif
}

#if CONFIG_RTW_PRE_LINK_STA
void rtw_pre_link_sta_del(struct sta_priv *stapriv, u8 *hwaddr)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct pre_link_sta_node_t *node = NULL;
	struct sta_info *sta = NULL;
	u8 exist = _FALSE;
	int i;
	_irqL irqL;

	if (rtw_check_invalid_mac_address(hwaddr, _FALSE) == _TRUE)
		goto exit;

	_enter_critical_bh(&(pre_link_sta_ctl->lock), &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, hwaddr, ETH_ALEN) == _TRUE
		) {
			node = &pre_link_sta_ctl->node[i];
			exist = _TRUE;
			break;
		}
	}

	if (exist == _TRUE && node) {
		node->valid = _FALSE;
		pre_link_sta_ctl->num--;
	}
	_exit_critical_bh(&(pre_link_sta_ctl->lock), &irqL);

	if (exist == _FALSE)
		goto exit;

	sta = rtw_get_stainfo(stapriv, hwaddr);
	if (!sta)
		goto exit;

	if (sta->state == WIFI_FW_PRE_LINK)
		rtw_free_stainfo(stapriv->padapter, sta);

exit:
	return;
}

void rtw_pre_link_sta_ctl_reset(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct sta_info *sta = NULL;
	int i, j = 0;
	_irqL irqL;

	u8 addrs[RTW_PRE_LINK_STA_NUM][ETH_ALEN];

	_rtw_memset(addrs, 0, RTW_PRE_LINK_STA_NUM * ETH_ALEN);

	_enter_critical_bh(&(pre_link_sta_ctl->lock), &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _FALSE)
			continue;
		_rtw_memcpy(&(addrs[j][0]), pre_link_sta_ctl->node[i].addr, ETH_ALEN);
		pre_link_sta_ctl->node[i].valid = _FALSE;
		pre_link_sta_ctl->num--;
		j++;
	}
	_exit_critical_bh(&(pre_link_sta_ctl->lock), &irqL);

	for (i = 0; i < j; i++) {
		sta = rtw_get_stainfo(stapriv, &(addrs[i][0]));
		if (!sta)
			continue;

		if (sta->state == WIFI_FW_PRE_LINK)
			rtw_free_stainfo(stapriv->padapter, sta);
	}
}

void rtw_pre_link_sta_ctl_init(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	int i;

	_rtw_spinlock_init(&pre_link_sta_ctl->lock);
	pre_link_sta_ctl->num = 0;
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++)
		pre_link_sta_ctl->node[i].valid = _FALSE;
}

void rtw_pre_link_sta_ctl_deinit(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;

	rtw_pre_link_sta_ctl_reset(stapriv);

	_rtw_spinlock_free(&pre_link_sta_ctl->lock);
}
#endif /* CONFIG_RTW_PRE_LINK_STA */

#endif /* !CONFIG_RUST || HOST_STA_MGT_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && CONFIG_RTW_MACADDR_ACL

struct wlan_acl_pool *rtw_rust_sta_acl_pool(_adapter *adapter, u8 period)
{
	if (period >= RTW_ACL_PERIOD_NUM)
		return NULL;
	return &adapter->stapriv.acl_list[period];
}

int rtw_rust_sta_acl_mode(struct wlan_acl_pool *acl)
{
	return acl ? acl->mode : RTW_ACL_MODE_DISABLED;
}

u8 rtw_rust_sta_acl_mac_listed(struct wlan_acl_pool *acl, const u8 *mac_addr)
{
	_irqL irqL;
	_list *list, *head;
	struct rtw_wlan_acl_node *acl_node;
	u8 match = _FALSE;
	_queue *acl_node_q;

	if (!acl || !mac_addr)
		return _FALSE;

	acl_node_q = &acl->acl_node_q;
	_enter_critical_bh(&(acl_node_q->lock), &irqL);
	head = get_list_head(acl_node_q);
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (_rtw_memcmp(acl_node->addr, mac_addr, ETH_ALEN)) {
			if (acl_node->valid == _TRUE) {
				match = _TRUE;
				break;
			}
		}
	}
	_exit_critical_bh(&(acl_node_q->lock), &irqL);
	return match;
}

void rtw_rust_sta_warn_on(int condition)
{
	rtw_warn_on(condition);
}

#endif /* CONFIG_RUST && !HOST_STA_MGT_TEST && CONFIG_RTW_MACADDR_ACL */

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST)

struct sta_priv *rtw_rust_sta_stapriv(_adapter *adapter)
{
	return &adapter->stapriv;
}

_adapter *rtw_rust_sta_adapter(struct sta_priv *stapriv)
{
	return stapriv->padapter;
}

u16 rtw_rust_sta_max_aid(struct sta_priv *stapriv)
{
	return stapriv->max_aid;
}

u16 rtw_rust_sta_max_num_sta(struct sta_priv *stapriv)
{
	return stapriv->max_num_sta;
}

u16 rtw_rust_sta_started_aid(struct sta_priv *stapriv)
{
	return stapriv->started_aid;
}

u8 rtw_rust_sta_rr_aid(struct sta_priv *stapriv)
{
	return stapriv->rr_aid;
}

struct sta_info *rtw_rust_sta_aid_entry(struct sta_priv *stapriv, u16 aid)
{
	if (!aid || aid > stapriv->max_aid)
		return NULL;
	return stapriv->sta_aid[aid - 1];
}

void rtw_rust_sta_aid_entry_set(struct sta_priv *stapriv, u16 aid, struct sta_info *sta)
{
	if (aid && aid <= stapriv->max_aid)
		stapriv->sta_aid[aid - 1] = sta;
}

void rtw_rust_sta_started_aid_set(struct sta_priv *stapriv, u16 aid)
{
	stapriv->started_aid = aid;
}

void rtw_rust_sta_cmn_aid_set(struct sta_info *sta, u16 aid)
{
	sta->cmn.aid = aid;
}

u32 rtw_rust_sta_state(struct sta_info *sta)
{
	return sta->state;
}

u8 rtw_rust_pre_link_find(struct sta_priv *stapriv, u8 *addr)
{
#if CONFIG_RTW_PRE_LINK_STA
	struct pre_link_sta_ctl_t *ctl = &stapriv->pre_link_sta_ctl;
	u8 found = _FALSE;
	int i;
	_irqL irqL;

	_enter_critical_bh(&ctl->lock, &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (ctl->node[i].valid == _TRUE
		    && _rtw_memcmp(ctl->node[i].addr, addr, ETH_ALEN) == _TRUE) {
			found = _TRUE;
			break;
		}
	}
	_exit_critical_bh(&ctl->lock, &irqL);
	return found;
#else
	return _FALSE;
#endif
}

u8 rtw_rust_pre_link_remove(struct sta_priv *stapriv, u8 *addr)
{
#if CONFIG_RTW_PRE_LINK_STA
	struct pre_link_sta_ctl_t *ctl = &stapriv->pre_link_sta_ctl;
	u8 removed = _FALSE;
	int i;
	_irqL irqL;

	_enter_critical_bh(&ctl->lock, &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (ctl->node[i].valid == _TRUE
		    && _rtw_memcmp(ctl->node[i].addr, addr, ETH_ALEN) == _TRUE) {
			ctl->node[i].valid = _FALSE;
			ctl->num--;
			removed = _TRUE;
			break;
		}
	}
	_exit_critical_bh(&ctl->lock, &irqL);
	return removed;
#else
	return _FALSE;
#endif
}

u8 rtw_rust_pre_link_drain(struct sta_priv *stapriv, u8 addrs[][ETH_ALEN], u8 max)
{
#if CONFIG_RTW_PRE_LINK_STA
	struct pre_link_sta_ctl_t *ctl = &stapriv->pre_link_sta_ctl;
	u8 n = 0;
	int i;
	_irqL irqL;

	_enter_critical_bh(&ctl->lock, &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM && n < max; i++) {
		if (ctl->node[i].valid == _FALSE)
			continue;
		_rtw_memcpy(addrs[n], ctl->node[i].addr, ETH_ALEN);
		ctl->node[i].valid = _FALSE;
		ctl->num--;
		n++;
	}
	_exit_critical_bh(&ctl->lock, &irqL);
	return n;
#else
	return 0;
#endif
}

#if CONFIG_RTW_PRE_LINK_STA
void rtw_rust_pre_link_ctl_init(struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *ctl = &stapriv->pre_link_sta_ctl;
	int i;

	_rtw_spinlock_init(&ctl->lock);
	ctl->num = 0;
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++)
		ctl->node[i].valid = _FALSE;
}

void rtw_rust_pre_link_ctl_lock_free(struct sta_priv *stapriv)
{
	_rtw_spinlock_free(&stapriv->pre_link_sta_ctl.lock);
}
#else
void rtw_rust_pre_link_ctl_init(struct sta_priv *stapriv)
{
	(void)stapriv;
}

void rtw_rust_pre_link_ctl_lock_free(struct sta_priv *stapriv)
{
	(void)stapriv;
}
#endif /* CONFIG_RTW_PRE_LINK_STA */

#endif /* CONFIG_RUST && !HOST_STA_MGT_TEST */
