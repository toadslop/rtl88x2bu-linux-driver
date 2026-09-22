// SPDX-License-Identifier: GPL-2.0
#include <stdlib.h>
#include <string.h>
#include "host_rm_fsm_types.h"

void *rtw_malloc(u32 sz) { return malloc(sz); }
void rtw_mfree(u8 *p, u32 sz) { (void)sz; free(p); }

static void rm_state_initial(struct rm_obj *prm) { prm->state = RM_ST_IDLE; }

int rm_enqueue_ev(_queue *queue, struct rm_event *obj, bool to_head)
{
	_irqL irqL;

	if (!obj)
		return _FAIL;
	_enter_critical(&queue->lock, &irqL);
	if (to_head)
		rtw_list_insert_head(&obj->list, &queue->queue);
	else
		rtw_list_insert_tail(&obj->list, &queue->queue);
	_exit_critical(&queue->lock, &irqL);
	return _SUCCESS;
}

void rm_free_rmobj(struct rm_obj *prm)
{
	if (is_list_linked(&prm->list))
		rtw_list_delete(&prm->list);
	if (prm->q.pssid)
		rtw_mfree(prm->q.pssid, strlen((char *)prm->q.pssid) + 1);
	if (prm->q.opt.bcn.req_start)
		rtw_mfree(prm->q.opt.bcn.req_start, prm->q.opt.bcn.req_len);
	if (prm->pclock)
		rm_free_clock(prm->pclock);
	rtw_mfree((u8 *)prm, sizeof(*prm));
}

struct rm_obj *rm_alloc_rmobj(_adapter *padapter)
{
	struct rm_obj *prm = rtw_malloc(sizeof(*prm));

	if (!prm)
		return NULL;
	_rtw_memset(prm, 0, sizeof(*prm));
	if (!(prm->pclock = rm_alloc_clock(padapter, prm))) {
		rm_free_rmobj(prm);
		return NULL;
	}
	return prm;
}

int rm_enqueue_rmobj(_adapter *padapter, struct rm_obj *prm, bool to_head)
{
	_irqL irqL;
	_queue *queue = &padapter->rmpriv.rm_queue;

	if (!prm)
		return _FAIL;
	_enter_critical(&queue->lock, &irqL);
	if (to_head)
		rtw_list_insert_head(&prm->list, &queue->queue);
	else
		rtw_list_insert_tail(&prm->list, &queue->queue);
	_exit_critical(&queue->lock, &irqL);
	rm_state_initial(prm);
	return _SUCCESS;
}
