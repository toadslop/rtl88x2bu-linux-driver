// SPDX-License-Identifier: GPL-2.0
/* Host ACL fixture helpers for W3-37 L2 oracle. */

#include "host_sta_mgt_types.h"

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
