// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-37/W3-38 L2 sta_mgt oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"

struct host_sta_slot {
	u8 used;
	u8 mac[ETH_ALEN];
	struct sta_info sta;
};

static struct host_sta_slot host_sta_pool[HOST_STA_MGT_MAX_STA];

static struct host_sta_slot *host_sta_mgt_find(const u8 *mac)
{
	int i;

	for (i = 0; i < HOST_STA_MGT_MAX_STA; i++) {
		if (host_sta_pool[i].used &&
		    _rtw_memcmp(host_sta_pool[i].mac, mac, ETH_ALEN) == _TRUE)
			return &host_sta_pool[i];
	}
	return NULL;
}

int rtw_check_invalid_mac_address(const u8 *mac, u8 check_local_bit)
{
	u8 zero[ETH_ALEN] = {0};
	u8 bcast[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	(void)check_local_bit;
	if (!mac)
		return _TRUE;
	if (_rtw_memcmp(mac, zero, ETH_ALEN) == _TRUE ||
	    _rtw_memcmp(mac, bcast, ETH_ALEN) == _TRUE)
		return _TRUE;
	if (mac[0] & 0x01)
		return _TRUE;
	return _FALSE;
}

struct sta_info *rtw_get_stainfo(struct sta_priv *stapriv, const u8 *hwaddr)
{
	struct host_sta_slot *slot;

	(void)stapriv;
	if (!hwaddr)
		return NULL;
	slot = host_sta_mgt_find(hwaddr);
	return slot ? &slot->sta : NULL;
}

void rtw_free_stainfo(_adapter *padapter, struct sta_info *psta)
{
	int i;
	struct sta_priv *stapriv;

	if (!psta)
		return;
	if (padapter && psta->cmn.aid > 0) {
		stapriv = &padapter->stapriv;
		if (stapriv->sta_aid &&
		    stapriv->sta_aid[psta->cmn.aid - 1] == psta)
			stapriv->sta_aid[psta->cmn.aid - 1] = NULL;
	}
	for (i = 0; i < HOST_STA_MGT_MAX_STA; i++) {
		if (&host_sta_pool[i].sta == psta) {
			memset(&host_sta_pool[i], 0, sizeof(host_sta_pool[i]));
			return;
		}
	}
}

void host_sta_mgt_acl_reset(_adapter *adapter)
{
	int p, i;

	for (p = 0; p < RTW_ACL_PERIOD_NUM; p++) {
		struct wlan_acl_pool *acl = &adapter->stapriv.acl_list[p];
		_queue *acl_node_q = &acl->acl_node_q;

		_rtw_init_listhead(&acl_node_q->queue);
		acl->num = 0;
		acl->mode = RTW_ACL_MODE_DISABLED;
		for (i = 0; i < NUM_ACL; i++) {
			_rtw_init_listhead(&acl->aclnode[i].list);
			acl->aclnode[i].valid = _FALSE;
		}
	}
}

void host_sta_mgt_acl_set_mode(_adapter *adapter, u8 period, int mode)
{
	if (period >= RTW_ACL_PERIOD_NUM)
		return;
	adapter->stapriv.acl_list[period].mode = mode;
}

int host_sta_mgt_acl_add(_adapter *adapter, u8 period, const u8 *addr)
{
	struct wlan_acl_pool *acl;
	_queue *acl_node_q;
	struct _list *head, *list;
	struct rtw_wlan_acl_node *acl_node;
	int i;

	if (period >= RTW_ACL_PERIOD_NUM)
		return -1;

	acl = &adapter->stapriv.acl_list[period];
	acl_node_q = &acl->acl_node_q;
	head = get_list_head(acl_node_q);
	list = get_next(head);

	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);
		if (_rtw_memcmp(acl_node->addr, addr, ETH_ALEN) &&
		    acl_node->valid == _TRUE)
			return 0;
	}

	if (acl->num >= NUM_ACL)
		return -1;

	for (i = 0; i < NUM_ACL; i++) {
		acl_node = &acl->aclnode[i];
		if (acl_node->valid == _FALSE) {
			_rtw_init_listhead(&acl_node->list);
			_rtw_memcpy(acl_node->addr, addr, ETH_ALEN);
			acl_node->valid = _TRUE;
			rtw_list_insert_tail(&acl_node->list, get_list_head(acl_node_q));
			acl->num++;
			return 0;
		}
	}

	return -1;
}

void host_sta_mgt_reset(_adapter *adapter)
{
	memset(host_sta_pool, 0, sizeof(host_sta_pool));
	if (adapter->stapriv.sta_aid)
		free(adapter->stapriv.sta_aid);
	memset(&adapter->stapriv, 0, sizeof(adapter->stapriv));
	adapter->stapriv.padapter = adapter;
	host_sta_mgt_acl_reset(adapter);
}

int host_sta_mgt_aid_setup(_adapter *adapter, u16 max_aid, u16 max_num_sta, u8 rr_aid)
{
	adapter->stapriv.max_aid = max_aid;
	adapter->stapriv.max_num_sta = max_num_sta;
	adapter->stapriv.started_aid = 1;
	adapter->stapriv.rr_aid = rr_aid;
	adapter->stapriv.sta_aid = calloc(max_aid, sizeof(struct sta_info *));
	return adapter->stapriv.sta_aid ? 0 : -1;
}

struct sta_info *host_sta_mgt_sta_add(_adapter *adapter, const u8 *mac, uint state)
{
	int i;
	struct host_sta_slot *slot = NULL;

	(void)adapter;
	for (i = 0; i < HOST_STA_MGT_MAX_STA; i++) {
		if (!host_sta_pool[i].used) {
			slot = &host_sta_pool[i];
			break;
		}
	}
	if (!slot)
		return NULL;

	slot->used = 1;
	_rtw_memcpy(slot->mac, mac, ETH_ALEN);
	_rtw_memcpy(slot->sta.cmn.mac_addr, mac, ETH_ALEN);
	slot->sta.state = state;
	return &slot->sta;
}

int host_sta_mgt_pre_link_add(_adapter *adapter, const u8 *mac)
{
	struct pre_link_sta_ctl_t *ctl = &adapter->stapriv.pre_link_sta_ctl;
	int i;
	_irqL irqL;

	_enter_critical_bh(&ctl->lock, &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (ctl->node[i].valid == _FALSE) {
			_rtw_memcpy(ctl->node[i].addr, mac, ETH_ALEN);
			ctl->node[i].valid = _TRUE;
			ctl->num++;
			_exit_critical_bh(&ctl->lock, &irqL);
			return 0;
		}
	}
	_exit_critical_bh(&ctl->lock, &irqL);
	return -1;
}
}

void host_sta_mgt_pre_link_init(_adapter *adapter)
{
	struct pre_link_sta_ctl_t *ctl = &adapter->stapriv.pre_link_sta_ctl;
	int i;

	_rtw_spinlock_init(&ctl->lock);
	ctl->num = 0;
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++)
		ctl->node[i].valid = _FALSE;
}

int host_sta_mgt_pre_link_count(_adapter *adapter)
{
	return adapter->stapriv.pre_link_sta_ctl.num;
}
