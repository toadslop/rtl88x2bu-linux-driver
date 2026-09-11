// SPDX-License-Identifier: GPL-2.0
#include <string.h>

#include "host_sta_mgt_types.h"

static struct sta_info host_alloc_sta_buf[NUM_STA];

void host_sta_mgt_alloc_reset(_adapter *adapter)
{
	int i;

	memset(host_alloc_sta_buf, 0, sizeof(host_alloc_sta_buf));
	memset(&adapter->stapriv, 0, sizeof(adapter->stapriv));
	adapter->stapriv.padapter = adapter;
	_rtw_spinlock_init(&adapter->stapriv.sta_hash_lock);
	for (i = 0; i < NUM_STA; i++)
		_rtw_init_listhead(&adapter->stapriv.sta_hash[i]);
}

int host_sta_mgt_alloc_setup(_adapter *adapter)
{
	struct sta_info *psta;
	int i;

	host_sta_mgt_alloc_reset(adapter);
	adapter->stapriv.pstainfo_buf = (u8 *)host_alloc_sta_buf;
	_rtw_init_queue(&adapter->stapriv.free_sta_queue);
	adapter->stapriv.asoc_sta_count = 0;

	psta = host_alloc_sta_buf;
	for (i = 0; i < NUM_STA; i++) {
		_rtw_init_stainfo(psta);
		rtw_list_insert_tail(&psta->list,
				     get_list_head(&adapter->stapriv.free_sta_queue));
		psta++;
	}
	return 0;
}

int host_sta_mgt_alloc_drain(_adapter *adapter, u8 count)
{
	struct sta_info *psta;
	u8 i;

	for (i = 0; i < count; i++) {
		if (_rtw_queue_empty(&adapter->stapriv.free_sta_queue))
			return -1;
		psta = LIST_CONTAINOR(get_next(&adapter->stapriv.free_sta_queue.queue),
				      struct sta_info, list);
		rtw_list_delete(&psta->list);
	}
	return 0;
}

int host_sta_mgt_alloc_free_count(_adapter *adapter)
{
	_list *head, *list;
	int count = 0;

	head = get_list_head(&adapter->stapriv.free_sta_queue);
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		count++;
		list = get_next(list);
	}
	return count;
}
