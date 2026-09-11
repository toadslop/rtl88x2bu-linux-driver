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
#define _RTW_STA_MGT_C_

#include <drv_types.h>

#define SESSION_TRACKER_FMT IP_FMT":"PORT_FMT" "IP_FMT":"PORT_FMT" %u %d"
#define SESSION_TRACKER_ARG(st) IP_ARG(&(st)->local_naddr), PORT_ARG(&(st)->local_port), IP_ARG(&(st)->remote_naddr), PORT_ARG(&(st)->remote_port), (st)->status, rtw_get_passing_time_ms((st)->set_time)

void dump_st_ctl(void *sel, struct st_ctl_t *st_ctl)
{
	int i;
	_irqL irqL;
	_list *plist, *phead;
	struct session_tracker *st;

	if (!DBG_SESSION_TRACKER)
		return;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
		RTW_PRINT_SEL(sel, "reg%d: %u %p\n", i, st_ctl->reg[i].s_proto, st_ctl->reg[i].rule);

	_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);

		RTW_PRINT_SEL(sel, SESSION_TRACKER_FMT"\n", SESSION_TRACKER_ARG(st));
	}
	_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);

}




/* using pstapriv->sta_hash_lock to protect */
u32	rtw_free_stainfo(_adapter *padapter , struct sta_info *psta)
{
	int i;
	_irqL irqL0;
	_queue *pfree_sta_queue, *pdefrag_q = NULL;
	struct recv_reorder_ctrl *preorder_ctrl;
	struct	sta_xmit_priv	*pstaxmitpriv;
	struct	xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct hw_xmit *phwxmit;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	int pending_qcnt[4];
	u8 is_pre_link_sta = _FALSE;
	_list	*phead, *plist;
	_queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;
	union recv_frame *prframe;

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


	pstaxmitpriv = &psta->sta_xmitpriv;

	/* rtw_list_delete(&psta->sleep_list); */

	/* rtw_list_delete(&psta->wakeup_list); */

	rtw_free_xmitframe_queue(pxmitpriv, &psta->tx_queue);
	_rtw_deinit_queue(&psta->tx_queue);

	_enter_critical_bh(&pxmitpriv->lock, &irqL0);

	rtw_free_xmitframe_queue(pxmitpriv, &psta->sleep_q);
	psta->sleepq_len = 0;

#ifdef CONFIG_RTW_MGMT_QUEUE
	rtw_free_mgmt_xmitframe_queue(pxmitpriv, &psta->mgmt_sleep_q);
	psta->mgmt_sleepq_len = 0;
#endif

	/* vo */
	/* _enter_critical_bh(&(pxmitpriv->vo_pending.lock), &irqL0); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vo_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vo_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits;
	phwxmit->accnt -= pstaxmitpriv->vo_q.qcnt;
	pending_qcnt[0] = pstaxmitpriv->vo_q.qcnt;
	pstaxmitpriv->vo_q.qcnt = 0;
	/* _exit_critical_bh(&(pxmitpriv->vo_pending.lock), &irqL0); */

	/* vi */
	/* _enter_critical_bh(&(pxmitpriv->vi_pending.lock), &irqL0); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->vi_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->vi_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 1;
	phwxmit->accnt -= pstaxmitpriv->vi_q.qcnt;
	pending_qcnt[1] = pstaxmitpriv->vi_q.qcnt;
	pstaxmitpriv->vi_q.qcnt = 0;
	/* _exit_critical_bh(&(pxmitpriv->vi_pending.lock), &irqL0); */

	/* be */
	/* _enter_critical_bh(&(pxmitpriv->be_pending.lock), &irqL0); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->be_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->be_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 2;
	phwxmit->accnt -= pstaxmitpriv->be_q.qcnt;
	pending_qcnt[2] = pstaxmitpriv->be_q.qcnt;
	pstaxmitpriv->be_q.qcnt = 0;
	/* _exit_critical_bh(&(pxmitpriv->be_pending.lock), &irqL0); */

	/* bk */
	/* _enter_critical_bh(&(pxmitpriv->bk_pending.lock), &irqL0); */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->bk_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->bk_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 3;
	phwxmit->accnt -= pstaxmitpriv->bk_q.qcnt;
	pending_qcnt[3] = pstaxmitpriv->bk_q.qcnt;
	pstaxmitpriv->bk_q.qcnt = 0;
	/* _exit_critical_bh(&(pxmitpriv->bk_pending.lock), &irqL0); */

#ifdef CONFIG_RTW_MGMT_QUEUE
	/* mgmt */
	rtw_free_xmitframe_queue(pxmitpriv, &pstaxmitpriv->mgmt_q.sta_pending);
	rtw_list_delete(&(pstaxmitpriv->mgmt_q.tx_pending));
	phwxmit = pxmitpriv->hwxmits + 4;
	phwxmit->accnt -= pstaxmitpriv->mgmt_q.qcnt;
	pstaxmitpriv->mgmt_q.qcnt = 0;
#endif

	rtw_os_wake_queue_at_free_stainfo(padapter, pending_qcnt);

	_exit_critical_bh(&pxmitpriv->lock, &irqL0);


	/* re-init sta_info; 20061114 */ /* will be init in alloc_stainfo */
	/* _rtw_init_sta_xmit_priv(&psta->sta_xmitpriv); */
	/* _rtw_init_sta_recv_priv(&psta->sta_recvpriv); */
#ifdef CONFIG_IEEE80211W
	_cancel_timer_ex(&psta->dot11w_expire_timer);
#endif /* CONFIG_IEEE80211W */
	_cancel_timer_ex(&psta->addba_retry_timer);

#ifdef CONFIG_TDLS
	psta->tdls_sta_state = TDLS_STATE_NONE;
#endif /* CONFIG_TDLS */

	/* for A-MPDU Rx reordering buffer control, cancel reordering_ctrl_timer */
	for (i = 0; i < 16 ; i++) {
		_irqL irqL;
		_queue *ppending_recvframe_queue;

		preorder_ctrl = &psta->recvreorder_ctrl[i];
		rtw_clear_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);

		_cancel_timer_ex(&preorder_ctrl->reordering_ctrl_timer);


		ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

		_enter_critical_bh(&ppending_recvframe_queue->lock, &irqL);

		phead =	get_list_head(ppending_recvframe_queue);
		plist = get_next(phead);

		while (!rtw_is_list_empty(phead)) {
			prframe = LIST_CONTAINOR(plist, union recv_frame, u);

			plist = get_next(plist);

			rtw_list_delete(&(prframe->u.hdr.list));

			rtw_free_recvframe(prframe, pfree_recv_queue);
		}

		_exit_critical_bh(&ppending_recvframe_queue->lock, &irqL);

	}

	/* CVE-2020-24586, clear defrag queue */
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


	if (!((psta->state & WIFI_AP_STATE) || MacAddr_isBcst(psta->cmn.mac_addr)) && is_pre_link_sta == _FALSE)
		rtw_hal_set_odm_var(padapter, HAL_ODM_STA_INFO, psta, _FALSE);


	/* release mac id for non-bc/mc station, */
	if (is_pre_link_sta == _FALSE)
		rtw_release_macid(pstapriv->padapter, psta);

#ifdef CONFIG_AP_MODE

	/*
		_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL0);
		rtw_list_delete(&psta->asoc_list);
		_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL0);
	*/
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

		/* rtw_indicate_sta_disassoc_event(padapter, psta); */

		if ((psta->cmn.aid > 0) && (pstapriv->sta_aid[psta->cmn.aid - 1] == psta)) {
			pstapriv->sta_aid[psta->cmn.aid - 1] = NULL;
			psta->cmn.aid = 0;
		}
	}

#endif /* CONFIG_NATIVEAP_MLME	 */

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
	psta->under_exist_checking = 0;
#endif

#endif /* CONFIG_AP_MODE	 */

	rtw_st_ctl_deinit(&psta->st_ctl);

	if (is_pre_link_sta == _FALSE) {
		_rtw_spinlock_free(&psta->lock);

		/* _enter_critical_bh(&(pfree_sta_queue->lock), &irqL0); */
		_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
		rtw_list_insert_tail(&psta->list, get_list_head(pfree_sta_queue));
		_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
		/* _exit_critical_bh(&(pfree_sta_queue->lock), &irqL0); */
	}

exit:
	return _SUCCESS;
}

/* free all stainfo which in sta_hash[all] */
void rtw_free_all_stainfo(_adapter *padapter)
{
	_irqL	 irqL;
	_list	*plist, *phead;
	s32	index;
	struct sta_info *psta = NULL;
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *pbcmc_stainfo = rtw_get_bcmc_stainfo(padapter);
	u8 free_sta_num = 0;
	char free_sta_list[NUM_STA];
	int stainfo_offset;


	if (pstapriv->asoc_sta_count == 1)
		goto exit;

	_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);

	for (index = 0; index < NUM_STA; index++) {
		phead = &(pstapriv->sta_hash[index]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info , hash_list);

			plist = get_next(plist);

			if (pbcmc_stainfo != psta) {
				if (rtw_is_pre_link_sta(pstapriv, psta->cmn.mac_addr) == _FALSE)
					rtw_list_delete(&psta->hash_list);

				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					free_sta_list[free_sta_num++] = stainfo_offset;
			}

		}
	}

	_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);


	for (index = 0; index < free_sta_num; index++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, free_sta_list[index]);
		rtw_free_stainfo(padapter , psta);
	}

exit:
	return;
}



struct sta_info *rtw_get_bcmc_stainfo(_adapter *padapter)
{
	struct sta_info	*psta;
	struct sta_priv	*pstapriv = &padapter->stapriv;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	psta = rtw_get_stainfo(pstapriv, bc_addr);
	return psta;

}

#ifdef CONFIG_AP_MODE
extern u16 rtw_aid_alloc(_adapter *adapter, struct sta_info *sta);

void dump_aid_status(void *sel, _adapter *adapter)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	u8 *aid_bmp;
	u16 i, used_cnt = 0;

	aid_bmp = rtw_zmalloc(stapriv->aid_bmp_len);
	if (!aid_bmp)
		return;

	for (i = 1; i <= stapriv->max_aid; i++) {
		if (stapriv->sta_aid[i - 1]) {
			aid_bmp[i / 8] |= BIT(i % 8);
			++used_cnt;
		}
	}

	RTW_PRINT_SEL(sel, "used_cnt:%u/%u\n", used_cnt, stapriv->max_aid);
	RTW_MAP_DUMP_SEL(sel, "aid_map:", aid_bmp, stapriv->aid_bmp_len);
	RTW_PRINT_SEL(sel, "\n");

	RTW_PRINT_SEL(sel, "%-2s %-11s\n", "rr", "started_aid");
	RTW_PRINT_SEL(sel, "%2d %11d\n", stapriv->rr_aid, stapriv->started_aid);

	rtw_mfree(aid_bmp, stapriv->aid_bmp_len);
}
#endif /* CONFIG_AP_MODE */

#if CONFIG_RTW_MACADDR_ACL
const char *const _acl_period_str[RTW_ACL_PERIOD_NUM] = {
	"DEV",
	"BSS",
};

const char *const _acl_mode_str[RTW_ACL_MODE_MAX] = {
	"DISABLED",
	"ACCEPT_UNLESS_LISTED",
	"DENY_UNLESS_LISTED",
};

void dump_macaddr_acl(void *sel, _adapter *adapter)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	int i, j;

	for (j = 0; j < RTW_ACL_PERIOD_NUM; j++) {
		RTW_PRINT_SEL(sel, "period:%s(%d)\n", acl_period_str(j), j);

		acl = &stapriv->acl_list[j];
		RTW_PRINT_SEL(sel, "mode:%s(%d)\n", acl_mode_str(acl->mode), acl->mode);
		RTW_PRINT_SEL(sel, "num:%d/%d\n", acl->num, NUM_ACL);
		for (i = 0; i < NUM_ACL; i++) {
			if (acl->aclnode[i].valid == _FALSE)
				continue;
			RTW_PRINT_SEL(sel, MAC_FMT"\n", MAC_ARG(acl->aclnode[i].addr));
		}
		RTW_PRINT_SEL(sel, "\n");
	}
}
#endif /* CONFIG_RTW_MACADDR_ACL */

extern bool rtw_is_pre_link_sta(struct sta_priv *stapriv, u8 *addr);

#if CONFIG_RTW_PRE_LINK_STA
extern void rtw_pre_link_sta_del(struct sta_priv *stapriv, u8 *hwaddr);
extern void rtw_pre_link_sta_ctl_reset(struct sta_priv *stapriv);
extern void rtw_pre_link_sta_ctl_init(struct sta_priv *stapriv);
extern void rtw_pre_link_sta_ctl_deinit(struct sta_priv *stapriv);

struct sta_info *rtw_pre_link_sta_add(struct sta_priv *stapriv, u8 *hwaddr)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	struct pre_link_sta_node_t *node = NULL;
	struct sta_info *sta = NULL;
	u8 exist = _FALSE;
	int i;
	_irqL irqL;

	if (rtw_check_invalid_mac_address(hwaddr, _FALSE) == _TRUE)
		goto exit;

	_enter_critical_bh(&(pre_link_sta_ctl->lock), &irqL);
	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _TRUE
			&& _rtw_memcmp(pre_link_sta_ctl->node[i].addr, hwaddr, ETH_ALEN) == _TRUE
		) {
			node = &pre_link_sta_ctl->node[i];
			exist = _TRUE;
			break;
		}

		if (node == NULL && pre_link_sta_ctl->node[i].valid == _FALSE)
			node = &pre_link_sta_ctl->node[i];
	}

	if (exist == _FALSE && node) {
		_rtw_memcpy(node->addr, hwaddr, ETH_ALEN);
		node->valid = _TRUE;
		pre_link_sta_ctl->num++;
	}
	_exit_critical_bh(&(pre_link_sta_ctl->lock), &irqL);

	if (node == NULL)
		goto exit;

	sta = rtw_get_stainfo(stapriv, hwaddr);
	if (sta)
		goto odm_hook;

	sta = rtw_alloc_stainfo(stapriv, hwaddr);
	if (!sta)
		goto exit;

	sta->state = WIFI_FW_PRE_LINK;

odm_hook:
	rtw_hal_set_odm_var(stapriv->padapter, HAL_ODM_STA_INFO, sta, _TRUE);

exit:
	return sta;
}

void dump_pre_link_sta_ctl(void *sel, struct sta_priv *stapriv)
{
	struct pre_link_sta_ctl_t *pre_link_sta_ctl = &stapriv->pre_link_sta_ctl;
	int i;

	RTW_PRINT_SEL(sel, "num:%d/%d\n", pre_link_sta_ctl->num, RTW_PRE_LINK_STA_NUM);

	for (i = 0; i < RTW_PRE_LINK_STA_NUM; i++) {
		if (pre_link_sta_ctl->node[i].valid == _FALSE)
			continue;
		RTW_PRINT_SEL(sel, MAC_FMT"\n", MAC_ARG(pre_link_sta_ctl->node[i].addr));
	}
}
#endif /* CONFIG_RTW_PRE_LINK_STA */

