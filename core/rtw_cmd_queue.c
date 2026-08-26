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
#include <hal_data.h>
#endif

#ifndef DBG_CMD_EXECUTE
#define DBG_CMD_EXECUTE 0
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

struct cmd_obj *_rtw_dequeue_cmd(_queue *queue)
{
	_irqL irqL;
	struct cmd_obj *obj;

	_enter_critical(&queue->lock, &irqL);

#ifdef DBG_CMD_QUEUE
	if (queue->queue.prev->next != &queue->queue) {
		RTW_INFO("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n", __LINE__,
			&queue->queue, queue->queue.prev, queue->queue.prev->prev->next, queue->queue.prev->next);
	}
#endif /* DBG_CMD_QUEUE */

	if (rtw_is_list_empty(&(queue->queue)))
		obj = NULL;
	else {
		obj = LIST_CONTAINOR(get_next(&(queue->queue)), struct cmd_obj, list);

#ifdef DBG_CMD_QUEUE
		if (queue->queue.prev->next != &queue->queue) {
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

		if (dump_cmd_id) {
			RTW_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
			if (obj->cmdcode == CMD_SET_DRV_EXTRA) {
				if (obj->parmbuf) {
					struct drvextra_cmd_parm *pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)(obj->parmbuf);

					printk("pdrvextra_cmd_parm->ec_id:0x%02x\n", pdrvextra_cmd_parm->ec_id);
				}
			}

		}
#endif /* DBG_CMD_QUEUE */

		rtw_list_delete(&obj->list);
	}

	_exit_critical(&queue->lock, &irqL);

	return obj;
}

int rtw_cmd_filter(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj)
{
#ifndef CONFIG_MAC_LOOPBACK_DRIVER
	u8 bAllow = _FALSE;
#else
	u8 bAllow = _TRUE;
#endif

#ifdef SUPPORT_HW_RFOFF_DETECTED
	if ((adapter_to_pwrctl(pcmdpriv->padapter)->bHWPwrPindetect)
	    && (!pcmdpriv->padapter->registrypriv.usbss_enable)
	   ) {
		if (cmd_obj->cmdcode == CMD_SET_DRV_EXTRA) {
			struct drvextra_cmd_parm *p = (struct drvextra_cmd_parm *)cmd_obj->parmbuf;

			if (p->ec_id == POWER_SAVING_CTRL_WK_CID)
				bAllow = _TRUE;
		}
	}
#endif

	if (cmd_obj->cmdcode == CMD_SET_CHANPLAN)
		bAllow = _TRUE;

	if (cmd_obj->no_io)
		bAllow = _TRUE;

	if ((!rtw_is_hw_init_completed(pcmdpriv->padapter) && (bAllow == _FALSE))
	    || ATOMIC_READ(&(pcmdpriv->cmdthd_running)) == _FALSE
	   ) {
		if (DBG_CMD_EXECUTE)
			RTW_INFO(ADPT_FMT" drop "CMD_FMT" hw_init_completed:%u, cmdthd_running:%u\n", ADPT_ARG(cmd_obj->padapter)
				, CMD_ARG(cmd_obj), rtw_get_hw_init_completed(cmd_obj->padapter), ATOMIC_READ(&pcmdpriv->cmdthd_running));
		if (0)
			rtw_warn_on(1);

		return _FAIL;
	}
	return _SUCCESS;
}

u32 rtw_enqueue_cmd(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj)
{
	int res = _FAIL;
	PADAPTER padapter = pcmdpriv->padapter;

	if (cmd_obj == NULL)
		goto exit;

	cmd_obj->padapter = padapter;

#ifdef CONFIG_CONCURRENT_MODE
	if (!is_primary_adapter(padapter))
		pcmdpriv = &(GET_PRIMARY_ADAPTER(padapter)->cmdpriv);
#endif

	res = rtw_cmd_filter(pcmdpriv, cmd_obj);
	if ((_FAIL == res) || (cmd_obj->cmdsz > MAX_CMDSZ)) {
		if (cmd_obj->cmdsz > MAX_CMDSZ) {
			RTW_INFO("%s failed due to obj->cmdsz(%d) > MAX_CMDSZ(%d)\n", __func__, cmd_obj->cmdsz, MAX_CMDSZ);
			rtw_warn_on(1);
		}

		if (cmd_obj->cmdcode == CMD_SET_DRV_EXTRA) {
			struct drvextra_cmd_parm *extra_parm = (struct drvextra_cmd_parm *)cmd_obj->parmbuf;

			if (extra_parm->pbuf && extra_parm->size > 0)
				rtw_mfree(extra_parm->pbuf, extra_parm->size);
		}
		rtw_free_cmd_obj(cmd_obj);
		goto exit;
	}

	res = _rtw_enqueue_cmd(&pcmdpriv->cmd_queue, cmd_obj, 0);

	if (res == _SUCCESS)
		_rtw_up_sema(&pcmdpriv->cmd_queue_sema);

exit:
	return res;
}

struct cmd_obj *rtw_dequeue_cmd(struct cmd_priv *pcmdpriv)
{
	return _rtw_dequeue_cmd(&pcmdpriv->cmd_queue);
}

void rtw_free_cmd_obj(struct cmd_obj *pcmd)
{
	if (pcmd->parmbuf != NULL)
		rtw_mfree((unsigned char *)pcmd->parmbuf, pcmd->cmdsz);
	if (pcmd->rsp != NULL) {
		if (pcmd->rspsz != 0)
			rtw_mfree((unsigned char *)pcmd->rsp, pcmd->rspsz);
	}

	rtw_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));
}

#ifdef CONFIG_EVENT_THREAD_MODE
u32 rtw_enqueue_evt(struct evt_priv *pevtpriv, struct evt_obj *obj)
{
	_irqL irqL;
	int res;
	_queue *queue = &pevtpriv->evt_queue;

	res = _SUCCESS;

	if (obj == NULL) {
		res = _FAIL;
		goto evt_exit;
	}

	_enter_critical_bh(&queue->lock, &irqL);

	rtw_list_insert_tail(&obj->list, &queue->queue);

	_exit_critical_bh(&queue->lock, &irqL);

evt_exit:
	return res;
}

void rtw_free_evt_obj(struct evt_obj *pevtobj)
{
	if (pevtobj->parmbuf)
		rtw_mfree((unsigned char *)pevtobj->parmbuf, pevtobj->evtsz);

	rtw_mfree((unsigned char *)pevtobj, sizeof(struct evt_obj));
}

void rtw_evt_notify_isr(struct evt_priv *pevtpriv)
{
	pevtpriv->evt_done_cnt++;
	_rtw_up_sema(&(pevtpriv->evt_notify));
}
#endif /* CONFIG_EVENT_THREAD_MODE */

#endif /* !CONFIG_RUST || HOST_CMD_QUEUE_TEST || !CONFIG_RUST_CMD_QUEUE */

#if defined(CONFIG_RUST) && defined(CONFIG_RUST_CMD_QUEUE) && !defined(HOST_CMD_QUEUE_TEST)
#include <hal_data.h>

u8 rtw_rust_hw_init_completed(void *adapter)
{
	return rtw_is_hw_init_completed((_adapter *)adapter);
}

void rtw_rust_queue_enter_critical(_lock *plock, _irqL *pirqL)
{
	_enter_critical(plock, pirqL);
}

void rtw_rust_queue_exit_critical(_lock *plock, _irqL *pirqL)
{
	_exit_critical(plock, pirqL);
}

void rtw_rust_queue_enter_critical_bh(_lock *plock, _irqL *pirqL)
{
	_enter_critical_bh(plock, pirqL);
}

void rtw_rust_queue_exit_critical_bh(_lock *plock, _irqL *pirqL)
{
	_exit_critical_bh(plock, pirqL);
}

void *rtw_rust_cmd_priv_padapter(struct cmd_priv *p)
{
	return p->padapter;
}

struct cmd_priv *rtw_rust_cmd_priv_for_enqueue(struct cmd_priv *pcmdpriv)
{
#ifdef CONFIG_CONCURRENT_MODE
	PADAPTER padapter = pcmdpriv->padapter;

	if (!is_primary_adapter(padapter))
		return &(GET_PRIMARY_ADAPTER(padapter)->cmdpriv);
#endif
	return pcmdpriv;
}

int rtw_rust_cmd_priv_cmdthd_running(struct cmd_priv *p)
{
	return ATOMIC_READ(&p->cmdthd_running);
}

#ifdef CONFIG_EVENT_THREAD_MODE
_queue *rtw_rust_evt_priv_evt_queue(struct evt_priv *p)
{
	return &p->evt_queue;
}

_sema *rtw_rust_evt_priv_evt_notify(struct evt_priv *p)
{
	return &p->evt_notify;
}

u32 *rtw_rust_evt_priv_evt_done_cnt(struct evt_priv *p)
{
	return &p->evt_done_cnt;
}
#endif /* CONFIG_EVENT_THREAD_MODE */
#endif
