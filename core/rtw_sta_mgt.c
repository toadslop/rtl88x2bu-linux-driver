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

