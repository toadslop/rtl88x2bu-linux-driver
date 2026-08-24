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
#define _RTW_CMD_PRIV_C_

#ifdef HOST_CMD_PRIV_TEST
#include "host_cmd_priv_types.h"
#else
#include <drv_types.h>
#endif

#ifdef CONFIG_C2H_WK
void c2h_wk_callback(_workitem *work);
#endif

#if !defined(CONFIG_RUST) || defined(HOST_CMD_PRIV_TEST) || !defined(CONFIG_RUST_CMD_PRIV)

sint _rtw_init_cmd_priv(struct cmd_priv *pcmdpriv)
{
	sint res = _SUCCESS;

	_rtw_init_sema(&(pcmdpriv->cmd_queue_sema), 0);
	_rtw_init_sema(&(pcmdpriv->start_cmdthread_sema), 0);

	_rtw_init_queue(&(pcmdpriv->cmd_queue));

	pcmdpriv->cmd_seq = 1;

	pcmdpriv->cmd_allocated_buf = rtw_zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

	if (pcmdpriv->cmd_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->cmd_buf = pcmdpriv->cmd_allocated_buf + CMDBUFF_ALIGN_SZ -
			    ((SIZE_PTR)(pcmdpriv->cmd_allocated_buf) & (CMDBUFF_ALIGN_SZ - 1));

	pcmdpriv->rsp_allocated_buf = rtw_zmalloc(MAX_RSPSZ + 4);

	if (pcmdpriv->rsp_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->rsp_buf = pcmdpriv->rsp_allocated_buf + 4 -
			    ((SIZE_PTR)(pcmdpriv->rsp_allocated_buf) & 3);

	pcmdpriv->cmd_issued_cnt = pcmdpriv->cmd_done_cnt = pcmdpriv->rsp_cnt = 0;

	_rtw_mutex_init(&pcmdpriv->sctx_mutex);
exit:
	return res;
}

void _rtw_free_cmd_priv(struct cmd_priv *pcmdpriv)
{
	if (pcmdpriv) {
		_rtw_spinlock_free(&(pcmdpriv->cmd_queue.lock));
		_rtw_free_sema(&(pcmdpriv->cmd_queue_sema));
		_rtw_free_sema(&(pcmdpriv->start_cmdthread_sema));

		if (pcmdpriv->cmd_allocated_buf)
			rtw_mfree(pcmdpriv->cmd_allocated_buf, MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

		if (pcmdpriv->rsp_allocated_buf)
			rtw_mfree(pcmdpriv->rsp_allocated_buf, MAX_RSPSZ + 4);

		_rtw_mutex_free(&pcmdpriv->sctx_mutex);
	}
}

sint _rtw_init_evt_priv(struct evt_priv *pevtpriv)
{
	sint res = _SUCCESS;

#ifdef CONFIG_H2CLBK
	_rtw_init_sema(&(pevtpriv->lbkevt_done), 0);
	pevtpriv->lbkevt_limit = 0;
	pevtpriv->lbkevt_num = 0;
	pevtpriv->cmdevt_parm = NULL;
#endif

	ATOMIC_SET(&pevtpriv->event_seq, 0);
	pevtpriv->evt_done_cnt = 0;

#ifdef CONFIG_EVENT_THREAD_MODE
	_rtw_init_sema(&(pevtpriv->evt_notify), 0);

	pevtpriv->evt_allocated_buf = rtw_zmalloc(MAX_EVTSZ + 4);
	if (pevtpriv->evt_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}
	pevtpriv->evt_buf = pevtpriv->evt_allocated_buf + 4 -
			    ((SIZE_PTR)(pevtpriv->evt_allocated_buf) & 3);

#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
	pevtpriv->allocated_c2h_mem = rtw_zmalloc(C2H_MEM_SZ + 4);

	if (pevtpriv->allocated_c2h_mem == NULL) {
		res = _FAIL;
		goto exit;
	}

	pevtpriv->c2h_mem = pevtpriv->allocated_c2h_mem + 4 -
			      ((u32)(pevtpriv->allocated_c2h_mem) & 3);
#endif

	_rtw_init_queue(&(pevtpriv->evt_queue));

exit:
#endif /* CONFIG_EVENT_THREAD_MODE */

#ifdef CONFIG_C2H_WK
	_init_workitem(&pevtpriv->c2h_wk, c2h_wk_callback, NULL);
	pevtpriv->c2h_wk_alive = _FALSE;
	pevtpriv->c2h_queue = rtw_cbuf_alloc(C2H_QUEUE_MAX_LEN + 1);
#endif

	return res;
}

void _rtw_free_evt_priv(struct evt_priv *pevtpriv)
{
#ifdef CONFIG_EVENT_THREAD_MODE
	_rtw_free_sema(&(pevtpriv->evt_notify));

	if (pevtpriv->evt_allocated_buf)
		rtw_mfree(pevtpriv->evt_allocated_buf, MAX_EVTSZ + 4);
#endif

#ifdef CONFIG_C2H_WK
	_cancel_workitem_sync(&pevtpriv->c2h_wk);
	while (pevtpriv->c2h_wk_alive)
		rtw_msleep_os(10);

	while (!rtw_cbuf_empty(pevtpriv->c2h_queue)) {
		void *c2h;

		c2h = rtw_cbuf_pop(pevtpriv->c2h_queue);
		if (c2h != NULL && c2h != (void *)pevtpriv)
			rtw_mfree(c2h, 16);
	}
	rtw_cbuf_free(pevtpriv->c2h_queue);
#endif
}

#endif /* !CONFIG_RUST || HOST_CMD_PRIV_TEST || !CONFIG_RUST_CMD_PRIV */
