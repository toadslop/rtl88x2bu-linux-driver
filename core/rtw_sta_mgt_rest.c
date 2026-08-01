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
