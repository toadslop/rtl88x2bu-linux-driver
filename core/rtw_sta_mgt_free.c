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
#define _RTW_STA_MGT_FREE_C_

#ifdef HOST_STA_MGT_TEST
#include "host_sta_mgt_types.h"
#else
#include <drv_types.h>
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE)
void rtw_mfree_stainfo(struct sta_info *psta);
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_STAINFO)
u32 rtw_free_stainfo(_adapter *padapter, struct sta_info *psta);
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_DEINIT)
u32 _rtw_free_sta_priv(struct sta_priv *pstapriv);
#endif

#if defined(RUST_STA_MGT_FREE_ORACLE)
void rtw_mfree_stainfo(struct sta_info *psta);
#else
void	_rtw_free_sta_xmit_priv_lock(struct sta_xmit_priv *psta_xmitpriv);
void	_rtw_free_sta_xmit_priv_lock(struct sta_xmit_priv *psta_xmitpriv)
{

	_rtw_spinlock_free(&psta_xmitpriv->lock);

	_rtw_spinlock_free(&(psta_xmitpriv->be_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->bk_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->vi_q.sta_pending.lock));
	_rtw_spinlock_free(&(psta_xmitpriv->vo_q.sta_pending.lock));
#ifdef CONFIG_RTW_MGMT_QUEUE
	_rtw_spinlock_free(&(psta_xmitpriv->mgmt_q.sta_pending.lock));
#endif
}

static void	_rtw_free_sta_recv_priv_lock(struct sta_recv_priv *psta_recvpriv)
{

	_rtw_spinlock_free(&psta_recvpriv->lock);

	_rtw_spinlock_free(&(psta_recvpriv->defrag_q.lock));


}

#if !defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST) || !defined(CONFIG_RUST_STA_MGT_FREE)
void rtw_mfree_stainfo(struct sta_info *psta);
void rtw_mfree_stainfo(struct sta_info *psta)
{

#ifndef HOST_STA_MGT_TEST
	if (&(psta->lock) != NULL)
#endif
		_rtw_spinlock_free(&psta->lock);

	_rtw_free_sta_xmit_priv_lock(&psta->sta_xmitpriv);
	_rtw_free_sta_recv_priv_lock(&psta->sta_recvpriv);

}
#endif /* !CONFIG_RUST || HOST_STA_MGT_TEST || !CONFIG_RUST_STA_MGT_FREE */

#endif /* RUST_STA_MGT_FREE_ORACLE */

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST)

void rtw_rust_mfree_stainfo_locks(struct sta_info *psta)
{
	if (psta == NULL)
		return;

	if (&(psta->lock) != NULL)
		_rtw_spinlock_free(&psta->lock);

	_rtw_free_sta_xmit_priv_lock(&psta->sta_xmitpriv);
	_rtw_free_sta_recv_priv_lock(&psta->sta_recvpriv);
}

#endif /* CONFIG_RUST && !HOST_STA_MGT_TEST */

#if defined(RUST_STA_MGT_FREE_ORACLE) && defined(RUST_STA_MGT_FREE_INIT_ORACLE)
u32 _rtw_init_sta_priv(struct sta_priv *pstapriv);
#endif

#if defined(RUST_STA_MGT_FREE_ORACLE) && defined(RUST_STA_MGT_FREE_DEINIT_ORACLE)
u32 _rtw_free_sta_priv(struct sta_priv *pstapriv);
#endif

#if defined(RUST_STA_MGT_FREE_ORACLE) && defined(RUST_STA_MGT_FREE_BCMc_ORACLE)
u32 rtw_init_bcmc_stainfo(_adapter *padapter);
#endif

#ifndef HOST_STA_MGT_TEST
void rtw_free_stainfo_flush_xmit(_adapter *padapter, struct sta_info *psta)
{
	_irqL irqL0;
	struct sta_xmit_priv *pstaxmitpriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct hw_xmit *phwxmit;
	int pending_qcnt[4];

	pstaxmitpriv = &psta->sta_xmitpriv;

	rtw_free_xmitframe_queue(pxmitpriv, &psta->tx_queue);
	_rtw_deinit_queue(&psta->tx_queue);

	_enter_critical_bh(&pxmitpriv->lock, &irqL0);

	rtw_free_xmitframe_queue(pxmitpriv, &psta->sleep_q);
	psta->sleepq_len = 0;

#ifdef CONFIG_RTW_MGMT_QUEUE
	rtw_free_mgmt_xmitframe_queue(pxmitpriv, &psta->mgmt_sleep_q);
	psta->mgmt_sleepq_len = 0;
#endif

	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vo_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vo_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits;
	phwxmit->accnt -= pstaxmitpriv->vo_q.qcnt;
	pending_qcnt[0] = pstaxmitpriv->vo_q.qcnt;
	pstaxmitpriv->vo_q.qcnt = 0;

	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vi_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vi_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 1;
	phwxmit->accnt -= pstaxmitpriv->vi_q.qcnt;
	pending_qcnt[1] = pstaxmitpriv->vi_q.qcnt;
	pstaxmitpriv->vi_q.qcnt = 0;

	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->be_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->be_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 2;
	phwxmit->accnt -= pstaxmitpriv->be_q.qcnt;
	pending_qcnt[2] = pstaxmitpriv->be_q.qcnt;
	pstaxmitpriv->be_q.qcnt = 0;

	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->bk_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->bk_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 3;
	phwxmit->accnt -= pstaxmitpriv->bk_q.qcnt;
	pending_qcnt[3] = pstaxmitpriv->bk_q.qcnt;
	pstaxmitpriv->bk_q.qcnt = 0;

#ifdef CONFIG_RTW_MGMT_QUEUE
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->mgmt_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->mgmt_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 4;
	phwxmit->accnt -= pstaxmitpriv->mgmt_q.qcnt;
	pstaxmitpriv->mgmt_q.qcnt = 0;
#endif

	rtw_os_wake_queue_at_free_stainfo(padapter, pending_qcnt);

	_exit_critical_bh(&pxmitpriv->lock, &irqL0);
}

void rtw_free_stainfo_flush_recv(_adapter *padapter, struct sta_info *psta)
{
	int i;
	_list *phead, *plist;
	_queue *pdefrag_q;
	_queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;
	union recv_frame *prframe;
	struct recv_reorder_ctrl *preorder_ctrl;

#ifdef CONFIG_IEEE80211W
	_cancel_timer_ex(&psta->dot11w_expire_timer);
#endif
	_cancel_timer_ex(&psta->addba_retry_timer);

#ifdef CONFIG_TDLS
	psta->tdls_sta_state = TDLS_STATE_NONE;
#endif

	for (i = 0; i < 16; i++) {
		_irqL irqL;
		_queue *ppending_recvframe_queue;

		preorder_ctrl = &psta->recvreorder_ctrl[i];
		rtw_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);
		_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);

		ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;
		_enter_critical_bh(&ppending_recvframe_queue->lock, &irqL);
		phead = get_list_head(ppending_recvframe_queue);
		plist = get_next(phead);
		while (!rtw_is_list_empty(phead)) {
			prframe = LIST_CONTAINOR(plist, union recv_frame, u);
			plist = get_next(plist);
			rtw_list_delete(&(prframe->u.hdr.list));
			rtw_free_recvframe(prframe, pfree_recv_queue);
		}
		_exit_critical_bh(&ppending_recvframe_queue->lock, &irqL);
	}

	pdefrag_q = &psta->sta_recvpriv.defrag_q;
	enter_critical_bh(&pdefrag_q->lock);
	phead = get_list_head(pdefrag_q);
	plist = get_next(phead);
	while (!rtw_is_list_empty(phead)) {
		prframe = LIST_CONTAINOR(plist, union recv_frame, u);
		plist = get_next(plist);
		rtw_list_delete(&(prframe->u.hdr.list));
		rtw_free_recvframe(prframe, pfree_recv_queue);
	}
	exit_critical_bh(&pdefrag_q->lock);
}

/* using pstapriv->sta_hash_lock to protect */
#if !defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_STAINFO_ORACLE) || \
	(defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_STAINFO))

static u32 rtw_free_stainfo_impl(_adapter *padapter, struct sta_info *psta)
{
	_irqL irqL0;
	_queue *pfree_sta_queue;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 is_pre_link_sta = _FALSE;

	if (psta == NULL)
		goto exit;

#ifdef CONFIG_RTW_80211K
	rm_post_event(padapter, RM_ID_FOR_ALL(psta->cmn.aid), RM_EV_cancel);
#endif

	is_pre_link_sta = rtw_is_pre_link_sta(pstapriv, psta->cmn.mac_addr);

	if (is_pre_link_sta == _FALSE) {
		_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
		rtw_list_delete(&psta->hash_list);
		pstapriv->asoc_sta_count--;
		_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
		rtw_mi_update_iface_status(&(padapter->mlmepriv), 0);
	} else {
		_enter_critical_bh(&psta->lock, &irqL0);
		psta->state = WIFI_FW_PRE_LINK;
		_exit_critical_bh(&psta->lock, &irqL0);
	}

	_enter_critical_bh(&psta->lock, &irqL0);
	psta->state &= ~WIFI_ASOC_STATE;
	_exit_critical_bh(&psta->lock, &irqL0);

	pfree_sta_queue = &pstapriv->free_sta_queue;

	rtw_free_stainfo_flush_xmit(padapter, psta);
	rtw_free_stainfo_flush_recv(padapter, psta);

	if (!((psta->state & WIFI_AP_STATE) || MacAddr_isBcst(psta->cmn.mac_addr)) &&
	    is_pre_link_sta == _FALSE)
		rtw_hal_set_odm_var(padapter, HAL_ODM_STA_INFO, psta, _FALSE);

	if (is_pre_link_sta == _FALSE)
		rtw_release_macid(pstapriv->padapter, psta);

#ifdef CONFIG_AP_MODE
	_enter_critical_bh(&pstapriv->auth_list_lock, &irqL0);
	if (!rtw_is_list_empty(&psta->auth_list)) {
		rtw_list_delete(&psta->auth_list);
		pstapriv->auth_list_cnt--;
	}
	_exit_critical_bh(&pstapriv->auth_list_lock, &irqL0);

	psta->expire_to = 0;
#ifdef CONFIG_ATMEL_RC_PATCH
	psta->flag_atmel_rc = 0;
#endif
	psta->sleepq_ac_len = 0;
	psta->qos_info = 0;
	psta->max_sp_len = 0;
	psta->uapsd_bk = 0;
	psta->uapsd_be = 0;
	psta->uapsd_vi = 0;
	psta->uapsd_vo = 0;
	psta->has_legacy_ac = 0;

#ifdef CONFIG_NATIVEAP_MLME
	if (pmlmeinfo->state == _HW_STATE_AP_) {
		rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, psta->cmn.aid);
		rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, psta->cmn.aid);

		if ((psta->cmn.aid > 0) &&
		    (pstapriv->sta_aid[psta->cmn.aid - 1] == psta)) {
			pstapriv->sta_aid[psta->cmn.aid - 1] = NULL;
			psta->cmn.aid = 0;
		}
	}
#endif

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
	psta->under_exist_checking = 0;
#endif
#endif /* CONFIG_AP_MODE */

	rtw_st_ctl_deinit(&psta->st_ctl);

	if (is_pre_link_sta == _FALSE) {
		_rtw_spinlock_free(&psta->lock);
		_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
		rtw_list_insert_tail(&psta->list, get_list_head(pfree_sta_queue));
		_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
	}

exit:
	return _SUCCESS;
}

#endif /* impl visibility */

#if (!defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_STAINFO_ORACLE)) && \
	(!defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST) || !defined(CONFIG_RUST_STA_MGT_FREE_STAINFO))
u32 rtw_free_stainfo(_adapter *padapter, struct sta_info *psta)
{
	return rtw_free_stainfo_impl(padapter, psta);
}
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_STAINFO)
u32 rtw_rust_free_stainfo_body(_adapter *padapter, struct sta_info *psta)
{
	return rtw_free_stainfo_impl(padapter, psta);
}
#endif

#endif /* !HOST_STA_MGT_TEST */

/* this function is used to free the memory of lock || sema for all stainfos */
void rtw_mfree_all_stainfo(struct sta_priv *pstapriv);
void rtw_mfree_all_stainfo(struct sta_priv *pstapriv)
{
	_irqL	 irqL;
	_list	*plist, *phead;
	struct sta_info *psta = NULL;


	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);

	phead = get_list_head(&pstapriv->free_sta_queue);
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info , list);
		plist = get_next(plist);

		rtw_mfree_stainfo(psta);
	}

	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);


}

void rtw_mfree_sta_priv_lock(struct	sta_priv *pstapriv);
void rtw_mfree_sta_priv_lock(struct	sta_priv *pstapriv)
{
	rtw_mfree_all_stainfo(pstapriv); /* be done before free sta_hash_lock */

	_rtw_spinlock_free(&pstapriv->free_sta_queue.lock);

	_rtw_spinlock_free(&pstapriv->sta_hash_lock);
	_rtw_spinlock_free(&pstapriv->wakeup_q.lock);
	_rtw_spinlock_free(&pstapriv->sleep_q.lock);

#ifdef CONFIG_AP_MODE
	_rtw_spinlock_free(&pstapriv->asoc_list_lock);
	_rtw_spinlock_free(&pstapriv->auth_list_lock);
#endif

}

#if !defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_INIT_ORACLE) || \
	(defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_INIT))

static u32 rtw_init_sta_priv_impl(struct sta_priv *pstapriv)
{
	_adapter *adapter = container_of(pstapriv, _adapter, stapriv);
	struct macid_ctl_t *macid_ctl = adapter_to_macidctl(adapter);
	struct sta_info *psta;
	s32 i;
	u32 ret = _FAIL;

	pstapriv->padapter = adapter;

	pstapriv->pallocated_stainfo_buf = rtw_zvmalloc(
		sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
	if (!pstapriv->pallocated_stainfo_buf)
		goto exit;

	pstapriv->pstainfo_buf = pstapriv->pallocated_stainfo_buf;
	if ((SIZE_PTR)pstapriv->pstainfo_buf & MEM_ALIGNMENT_PADDING)
		pstapriv->pstainfo_buf += MEM_ALIGNMENT_OFFSET -
			((SIZE_PTR)pstapriv->pstainfo_buf & MEM_ALIGNMENT_PADDING);

	_rtw_init_queue(&pstapriv->free_sta_queue);

	_rtw_spinlock_init(&pstapriv->sta_hash_lock);

	/* _rtw_init_queue(&pstapriv->asoc_q); */
	pstapriv->asoc_sta_count = 0;
	_rtw_init_queue(&pstapriv->sleep_q);
	_rtw_init_queue(&pstapriv->wakeup_q);

	psta = (struct sta_info *)(pstapriv->pstainfo_buf);


	for (i = 0; i < NUM_STA; i++) {
		_rtw_init_stainfo(psta);

		_rtw_init_listhead(&(pstapriv->sta_hash[i]));

		rtw_list_insert_tail(&psta->list, get_list_head(&pstapriv->free_sta_queue));

		psta++;
	}

	pstapriv->adhoc_expire_to = 4; /* 4 * 2 = 8 sec */

#ifdef CONFIG_AP_MODE
	pstapriv->max_aid = macid_ctl->num;
	pstapriv->rr_aid = 0;
	pstapriv->started_aid = 1;
	pstapriv->sta_aid = rtw_zmalloc(pstapriv->max_aid * sizeof(struct sta_info *));
	if (!pstapriv->sta_aid)
		goto exit;
	pstapriv->aid_bmp_len = AID_BMP_LEN(pstapriv->max_aid);
	pstapriv->sta_dz_bitmap = rtw_zmalloc(pstapriv->aid_bmp_len);
	if (!pstapriv->sta_dz_bitmap)
		goto exit;
	pstapriv->tim_bitmap = rtw_zmalloc(pstapriv->aid_bmp_len);
	if (!pstapriv->tim_bitmap)
		goto exit;

	_rtw_init_listhead(&pstapriv->asoc_list);
	_rtw_init_listhead(&pstapriv->auth_list);
	_rtw_spinlock_init(&pstapriv->asoc_list_lock);
	_rtw_spinlock_init(&pstapriv->auth_list_lock);
	pstapriv->asoc_list_cnt = 0;
	pstapriv->auth_list_cnt = 0;
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	pstapriv->tbtx_asoc_list_cnt = 0;
#endif

	pstapriv->auth_to = 3; /* 3*2 = 6 sec */
	pstapriv->assoc_to = 3;
	/* pstapriv->expire_to = 900; */ /* 900*2 = 1800 sec = 30 min, expire after no any traffic. */
	/* pstapriv->expire_to = 30; */ /* 30*2 = 60 sec = 1 min, expire after no any traffic. */
#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	pstapriv->expire_to = 3; /* 3*2 = 6 sec */
#else
	pstapriv->expire_to = 60;/* 60*2 = 120 sec = 2 min, expire after no any traffic. */
#endif
#ifdef CONFIG_ATMEL_RC_PATCH
	_rtw_memset(pstapriv->atmel_rc_pattern, 0, ETH_ALEN);
#endif
	pstapriv->max_num_sta = NUM_STA;

#if CONFIG_RTW_MACADDR_ACL
	for (i = 0; i < RTW_ACL_PERIOD_NUM; i++)
		rtw_macaddr_acl_init(adapter, i);
#endif
#endif /* CONFIG_AP_MODE */

#if CONFIG_RTW_PRE_LINK_STA
	rtw_pre_link_sta_ctl_init(pstapriv);
#endif

#if defined(DBG_ROAMING_TEST) || defined(CONFIG_RTW_REPEATER_SON)
	rtw_set_rx_chk_limit(adapter,1);
#elif defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && !defined(CONFIG_LPS_LCLK_WD_TIMER)
	rtw_set_rx_chk_limit(adapter,4);
#else
	rtw_set_rx_chk_limit(adapter,8);
#endif

	ret = _SUCCESS;

exit:
	if (ret != _SUCCESS) {
		if (pstapriv->pallocated_stainfo_buf)
			rtw_vmfree(pstapriv->pallocated_stainfo_buf,
				sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
		#ifdef CONFIG_AP_MODE
		if (pstapriv->sta_aid)
			rtw_mfree((u8 *)pstapriv->sta_aid,
				  pstapriv->max_aid * sizeof(struct sta_info *));
		if (pstapriv->sta_dz_bitmap)
			rtw_mfree(pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
		#endif
	}

	return ret;
}

#endif /* impl visibility */

#if (!defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_INIT_ORACLE)) && \
	(!defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST) || !defined(CONFIG_RUST_STA_MGT_FREE_INIT))
u32	_rtw_init_sta_priv(struct	sta_priv *pstapriv)
{
	return rtw_init_sta_priv_impl(pstapriv);
}
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_INIT)
u32 rtw_rust_init_sta_priv_body(struct sta_priv *pstapriv)
{
	return rtw_init_sta_priv_impl(pstapriv);
}
#endif

#if !defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_DEINIT_ORACLE) || \
	(defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_DEINIT))

static u32 rtw_free_sta_priv_impl(struct sta_priv *pstapriv)
{
	_irqL	irqL;
	_list	*phead, *plist;
	struct sta_info *psta = NULL;
	struct recv_reorder_ctrl *preorder_ctrl;
	int	index;

	if (pstapriv) {

		/*	delete all reordering_ctrl_timer		*/
		_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);
		for (index = 0; index < NUM_STA; index++) {
			phead = &(pstapriv->sta_hash[index]);
			plist = get_next(phead);

			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				int i;
				psta = LIST_CONTAINOR(plist, struct sta_info , hash_list);
				plist = get_next(plist);

				for (i = 0; i < 16 ; i++) {
					preorder_ctrl = &psta->recvreorder_ctrl[i];
					_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);
				}
			}
		}
		_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);
		/*===============================*/

		rtw_mfree_sta_priv_lock(pstapriv);

#if CONFIG_RTW_MACADDR_ACL
		for (index = 0; index < RTW_ACL_PERIOD_NUM; index++)
			rtw_macaddr_acl_deinit(pstapriv->padapter, index);
#endif

#if CONFIG_RTW_PRE_LINK_STA
		rtw_pre_link_sta_ctl_deinit(pstapriv);
#endif

		if (pstapriv->pallocated_stainfo_buf)
			rtw_vmfree(pstapriv->pallocated_stainfo_buf,
				sizeof(struct sta_info) * NUM_STA + MEM_ALIGNMENT_OFFSET);
		#ifdef CONFIG_AP_MODE
		if (pstapriv->sta_aid)
			rtw_mfree((u8 *)pstapriv->sta_aid,
				  pstapriv->max_aid * sizeof(struct sta_info *));
		if (pstapriv->sta_dz_bitmap)
			rtw_mfree(pstapriv->sta_dz_bitmap, pstapriv->aid_bmp_len);
		if (pstapriv->tim_bitmap)
			rtw_mfree(pstapriv->tim_bitmap, pstapriv->aid_bmp_len);
		#endif
	}

	return _SUCCESS;
}

#endif /* impl visibility */

#if (!defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_DEINIT_ORACLE)) && \
	(!defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST) || !defined(CONFIG_RUST_STA_MGT_FREE_DEINIT))
u32	_rtw_free_sta_priv(struct	sta_priv *pstapriv)
{
	return rtw_free_sta_priv_impl(pstapriv);
}
#endif

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST) && defined(CONFIG_RUST_STA_MGT_FREE_DEINIT)
u32 rtw_rust_free_sta_priv_body(struct sta_priv *pstapriv)
{
	return rtw_free_sta_priv_impl(pstapriv);
}
#endif

#if !defined(RUST_STA_MGT_FREE_ORACLE) || !defined(RUST_STA_MGT_FREE_BCMc_ORACLE)
u32 rtw_init_bcmc_stainfo(_adapter *padapter)
{

	struct sta_info	*psta;
#ifdef HOST_STA_MGT_TEST
	u8 bcast_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#else
	struct tx_servq	*ptxservq;
	u32 res = _SUCCESS;
	NDIS_802_11_MAC_ADDRESS	bcast_addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	struct	sta_priv *pstapriv = &padapter->stapriv;


	psta = rtw_alloc_stainfo(pstapriv, bcast_addr);

	if (psta == NULL) {
#ifndef HOST_STA_MGT_TEST
		res = _FAIL;
#endif
		goto exit;
	}
#ifndef HOST_STA_MGT_TEST
#ifdef CONFIG_BEAMFORMING
	psta->cmn.bf_info.g_id = 63;
	psta->cmn.bf_info.p_aid = 0;
#endif

	ptxservq = &(psta->sta_xmitpriv.be_q);

	/*
		_enter_critical(&pstapending->lock, &irqL0);

		if (rtw_is_list_empty(&ptxservq->tx_pending))
			rtw_list_insert_tail(&ptxservq->tx_pending, get_list_head(pstapending));

		_exit_critical(&pstapending->lock, &irqL0);
	*/
#endif /* !HOST_STA_MGT_TEST */

exit:
	return _SUCCESS;

}
#endif /* !RUST_STA_MGT_FREE_ORACLE || !RUST_STA_MGT_FREE_BCMc_ORACLE */
