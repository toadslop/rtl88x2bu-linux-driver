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
#define _RTW_STA_MGT_ALLOC_C_

#ifdef HOST_STA_MGT_TEST
#include "host_sta_mgt_types.h"
#else
#include <drv_types.h>
#endif

#ifndef HOST_STA_MGT_TEST
static void rtw_init_recv_timer(struct recv_reorder_ctrl *preorder_ctrl)
{
	_adapter *padapter = preorder_ctrl->padapter;

#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
	rtw_init_timer(&(preorder_ctrl->reordering_ctrl_timer), padapter,
		       rtw_reordering_ctrl_timeout_handler, preorder_ctrl);
#endif
}
#endif /* !HOST_STA_MGT_TEST */

struct sta_info *rtw_alloc_stainfo(struct sta_priv *pstapriv, const u8 *hwaddr)
{
	_irqL irqL2;
	s32	index;
	_list	*phash_list;
	struct sta_info	*psta;
	_queue *pfree_sta_queue;
#ifndef HOST_STA_MGT_TEST
	struct recv_reorder_ctrl *preorder_ctrl;
#endif
#ifndef HOST_STA_MGT_TEST
	int i = 0;
	u16  wRxSeqInitialValue = 0xffff;
#endif

	pfree_sta_queue = &pstapriv->free_sta_queue;
	_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL2);
	if (_rtw_queue_empty(pfree_sta_queue) == _TRUE) {
		psta = NULL;
	} else {
		psta = LIST_CONTAINOR(get_next(&pfree_sta_queue->queue), struct sta_info, list);
		rtw_list_delete(&(psta->list));
		_rtw_init_stainfo(psta);
		psta->padapter = pstapriv->padapter;
		_rtw_memcpy(psta->cmn.mac_addr, hwaddr, ETH_ALEN);
		index = wifi_mac_hash(hwaddr);
		if (index >= NUM_STA) {
			psta = NULL;
			goto exit;
		}
		phash_list = &(pstapriv->sta_hash[index]);
		rtw_list_insert_tail(&psta->hash_list, phash_list);
		pstapriv->asoc_sta_count++;
#ifndef HOST_STA_MGT_TEST
		for (i = 0; i < 16; i++) {
			_rtw_memcpy(&psta->sta_recvpriv.rxcache.tid_rxseq[i], &wRxSeqInitialValue, 2);
			_rtw_memcpy(&psta->sta_recvpriv.bmc_tid_rxseq[i], &wRxSeqInitialValue, 2);
			_rtw_memset(&psta->sta_recvpriv.rxcache.iv[i], 0, sizeof(psta->sta_recvpriv.rxcache.iv[i]));
		}
		_rtw_memcpy(&psta->sta_recvpriv.nonqos_bmc_rxseq,&wRxSeqInitialValue,2);
		_rtw_memcpy(&psta->sta_recvpriv.nonqos_rxseq,&wRxSeqInitialValue,2);

		rtw_init_timer(&psta->addba_retry_timer, psta->padapter, addba_timer_hdl, psta);
#ifdef CONFIG_IEEE80211W
		rtw_init_timer(&psta->dot11w_expire_timer, psta->padapter, sa_query_timer_hdl, psta);
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_TDLS
		rtw_init_tdls_timer(pstapriv->padapter, psta);
#endif /* CONFIG_TDLS */

		for (i = 0; i < 16 ; i++) {
			preorder_ctrl = &psta->recvreorder_ctrl[i];
			preorder_ctrl->padapter = pstapriv->padapter;
			preorder_ctrl->tid = i;
			preorder_ctrl->enable = _FALSE;
			preorder_ctrl->indicate_seq = 0xffff;
			preorder_ctrl->wend_b = 0xffff;
			preorder_ctrl->wsize_b = 64;
			preorder_ctrl->ampdu_size = RX_AMPDU_SIZE_INVALID;

			_rtw_init_queue(&preorder_ctrl->pending_recvframe_queue);
#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
			rtw_init_timer(&(preorder_ctrl->reordering_ctrl_timer),
				       pstapriv->padapter,
				       rtw_reordering_ctrl_timeout_handler,
				       preorder_ctrl);
#endif
			rtw_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);
		}
		ATOMIC_SET(&psta->keytrack, 0);
		psta->cmn.rssi_stat.rssi = (-1);
		psta->cmn.rssi_stat.rssi_cck = (-1);
		psta->cmn.rssi_stat.rssi_ofdm = (-1);
#ifdef CONFIG_ATMEL_RC_PATCH
		psta->flag_atmel_rc = 0;
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
		psta->tbtx_enable = _FALSE;
#endif
		psta->RxMgmtFrameSeqNum = 0xffff;
		_rtw_memset(&psta->sta_stats, 0, sizeof(struct stainfo_stats));

		rtw_alloc_macid(pstapriv->padapter, psta);

		psta->tx_q_enable = 0;
		_rtw_init_queue(&psta->tx_queue);
		_init_workitem(&psta->tx_q_work, rtw_xmit_dequeue_callback, NULL);
#endif /* !HOST_STA_MGT_TEST */
	}

exit:
	_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL2);
#ifndef HOST_STA_MGT_TEST
	if (psta)
		rtw_mi_update_iface_status(&(pstapriv->padapter->mlmepriv), 0);
#endif

	return psta;
}
