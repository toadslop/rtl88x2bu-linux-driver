// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-55 st_ctl L2 oracles. */

#include <stdlib.h>
#include <string.h>

#include "host_sta_mgt_types.h"

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_mfree(u8 *pbuf, u32 sz)
{
	(void)sz;
	free(pbuf);
}

static struct sta_info host_offset_sta_pool[HOST_STA_MGT_NUM_STA];

void host_sta_mgt_stctl_reset(struct st_ctl_t *st_ctl)
{
	memset(st_ctl, 0, sizeof(*st_ctl));
	rtw_st_ctl_init(st_ctl);
}

int host_sta_mgt_stctl_tracker_count(struct st_ctl_t *st_ctl)
{
	_irqL irqL;
	_list *plist, *phead;
	int count = 0;

	_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		count++;
		plist = get_next(plist);
	}
	_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);
	return count;
}

void host_sta_mgt_stctl_tracker_add(struct st_ctl_t *st_ctl)
{
	struct session_tracker *st = rtw_zmalloc(sizeof(*st));

	if (!st)
		return;
	_rtw_init_listhead(&st->list);
	rtw_list_insert_tail(&st->list, &st_ctl->tracker_q.queue);
}

int host_sta_mgt_offset_setup(_adapter *adapter, u8 sta_index, struct sta_info **out_sta)
{
	if (!adapter || sta_index >= HOST_STA_MGT_NUM_STA)
		return -1;
	adapter->stapriv.pstainfo_buf = (u8 *)host_offset_sta_pool;
	*out_sta = &host_offset_sta_pool[sta_index];
	memset(*out_sta, 0, sizeof(**out_sta));
	return 0;
}
