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

u32	_rtw_init_sta_priv(struct	sta_priv *pstapriv)
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

u32	_rtw_free_sta_priv(struct	sta_priv *pstapriv)
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
