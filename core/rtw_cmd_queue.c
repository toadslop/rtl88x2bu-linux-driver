/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#define _RTW_CMD_QUEUE_C_

#ifdef HOST_CMD_QUEUE_TEST
#include "host_cmd_queue_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_CMD_QUEUE_TEST) || !defined(CONFIG_RUST_CMD_QUEUE)

#ifdef DBG_CMD_QUEUE
extern u8 dump_cmd_id;
#endif

sint _rtw_enqueue_cmd(_queue *queue, struct cmd_obj *obj, bool to_head)
{
	_irqL irqL;

	if (obj == NULL)
		goto exit;

	_enter_critical(&queue->lock, &irqL);

	if (to_head)
		rtw_list_insert_head(&obj->list, &queue->queue);
	else
		rtw_list_insert_tail(&obj->list, &queue->queue);

#ifdef DBG_CMD_QUEUE
	if (dump_cmd_id) {
		RTW_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
		if (obj->cmdcode == CMD_SET_MLME_EVT) {
			if (obj->parmbuf) {
				struct rtw_evt_header *evt_hdr = (struct rtw_evt_header *)(obj->parmbuf);

				RTW_INFO("evt_hdr->id:%d\n", evt_hdr->id);
			}
		}
		if (obj->cmdcode == CMD_SET_DRV_EXTRA) {
			if (obj->parmbuf) {
				struct drvextra_cmd_parm *pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)(obj->parmbuf);

				RTW_INFO("pdrvextra_cmd_parm->ec_id:0x%02x\n", pdrvextra_cmd_parm->ec_id);
			}
		}
	}

	if (queue->queue.prev->next != &queue->queue) {
		RTW_INFO("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n", __LINE__,
			&queue->queue, queue->queue.prev, queue->queue.prev->prev->next, queue->queue.prev->next);

		RTW_INFO("==========%s============\n", __FUNCTION__);
		RTW_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
		RTW_INFO("padapter: %p\n", obj->padapter);
		RTW_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
		RTW_INFO("res: %d\n", obj->res);
		RTW_INFO("parmbuf: %p\n", obj->parmbuf);
		RTW_INFO("cmdsz: %d\n", obj->cmdsz);
		RTW_INFO("rsp: %p\n", obj->rsp);
		RTW_INFO("rspsz: %d\n", obj->rspsz);
		RTW_INFO("sctx: %p\n", obj->sctx);
		RTW_INFO("list->next: %p\n", obj->list.next);
		RTW_INFO("list->prev: %p\n", obj->list.prev);
	}
#endif /* DBG_CMD_QUEUE */

	_exit_critical(&queue->lock, &irqL);

	exit:
	return _SUCCESS;
}

#endif /* !CONFIG_RUST || HOST_CMD_QUEUE_TEST || !CONFIG_RUST_CMD_QUEUE */
