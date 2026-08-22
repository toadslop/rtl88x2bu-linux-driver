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
#define _RTW_STA_MGT_STCTL_C_

#ifdef HOST_STA_MGT_TEST
#include "host_sta_mgt_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_STA_MGT_TEST) || !defined(CONFIG_RUST_STA_MGT_STCTL)

bool test_st_match_rule(_adapter *adapter, u8 *local_naddr, u8 *local_port,
			u8 *remote_naddr, u8 *remote_port);

struct st_register test_st_reg = {
	.s_proto = 0x06,
	.rule = test_st_match_rule,
};

#if defined(HOST_STA_MGT_TEST)
#define STCTL_API
#else
#define STCTL_API inline
#endif

STCTL_API void rtw_st_ctl_init(struct st_ctl_t *st_ctl)
{
	_rtw_memset(st_ctl->reg, 0, sizeof(struct st_register) * SESSION_TRACKER_REG_ID_NUM);
	_rtw_init_queue(&st_ctl->tracker_q);
}

STCTL_API void rtw_st_ctl_clear_tracker_q(struct st_ctl_t *st_ctl)
{
	_irqL irqL;
	_list *plist, *phead;
	struct session_tracker *st;

	_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);
		rtw_list_delete(&st->list);
		rtw_mfree((u8 *)st, sizeof(struct session_tracker));
	}
	_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);
}

STCTL_API void rtw_st_ctl_deinit(struct st_ctl_t *st_ctl)
{
	rtw_st_ctl_clear_tracker_q(st_ctl);
	_rtw_deinit_queue(&st_ctl->tracker_q);
}

STCTL_API void rtw_st_ctl_register(struct st_ctl_t *st_ctl, u8 st_reg_id, struct st_register *reg)
{
	if (st_reg_id >= SESSION_TRACKER_REG_ID_NUM) {
		rtw_warn_on(1);
		return;
	}

	st_ctl->reg[st_reg_id].s_proto = reg->s_proto;
	st_ctl->reg[st_reg_id].rule = reg->rule;
}

STCTL_API void rtw_st_ctl_unregister(struct st_ctl_t *st_ctl, u8 st_reg_id)
{
	int i;

	if (st_reg_id >= SESSION_TRACKER_REG_ID_NUM) {
		rtw_warn_on(1);
		return;
	}

	st_ctl->reg[st_reg_id].s_proto = 0;
	st_ctl->reg[st_reg_id].rule = NULL;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
		if (st_ctl->reg[i].s_proto != 0)
			break;
	if (i >= SESSION_TRACKER_REG_ID_NUM)
		rtw_st_ctl_clear_tracker_q(st_ctl);
}

STCTL_API bool rtw_st_ctl_chk_reg_s_proto(struct st_ctl_t *st_ctl, u8 s_proto)
{
	bool ret = _FALSE;
	int i;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		if (st_ctl->reg[i].s_proto == s_proto) {
			ret = _TRUE;
			break;
		}
	}

	return ret;
}

STCTL_API bool rtw_st_ctl_chk_reg_rule(struct st_ctl_t *st_ctl, _adapter *adapter, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	bool ret = _FALSE;
	int i;
	st_match_rule rule;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		rule = st_ctl->reg[i].rule;
		if (rule && rule(adapter, local_naddr, local_port, remote_naddr, remote_port) == _TRUE) {
			ret = _TRUE;
			break;
		}
	}

	return ret;
}

STCTL_API int rtw_stainfo_offset(struct sta_priv *stapriv, struct sta_info *sta)
{
	int offset = (((u8 *)sta) - stapriv->pstainfo_buf) / sizeof(struct sta_info);

	if (!stainfo_offset_valid(offset))
		RTW_INFO("%s invalid offset(%d), out of range!!!", __func__, offset);

	return offset;
}

#endif /* !CONFIG_RUST || HOST_STA_MGT_TEST || !CONFIG_RUST_STA_MGT_STCTL */

#if defined(CONFIG_RUST) && !defined(HOST_STA_MGT_TEST)

void rtw_rust_stctl_memzero_reg(struct st_ctl_t *st_ctl)
{
	_rtw_memset(st_ctl->reg, 0, sizeof(struct st_register) * SESSION_TRACKER_REG_ID_NUM);
}

void rtw_rust_stctl_queue_init(struct st_ctl_t *st_ctl)
{
	_rtw_init_queue(&st_ctl->tracker_q);
}

void rtw_rust_stctl_queue_deinit(struct st_ctl_t *st_ctl)
{
	_rtw_deinit_queue(&st_ctl->tracker_q);
}

void rtw_rust_stctl_clear_tracker_q(struct st_ctl_t *st_ctl)
{
	_irqL irqL;
	_list *plist, *phead;
	struct session_tracker *st;

	_enter_critical_bh(&st_ctl->tracker_q.lock, &irqL);
	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);
		rtw_list_delete(&st->list);
		rtw_mfree((u8 *)st, sizeof(struct session_tracker));
	}
	_exit_critical_bh(&st_ctl->tracker_q.lock, &irqL);
}

u8 *rtw_rust_stainfo_buf(struct sta_priv *stapriv)
{
	return stapriv->pstainfo_buf;
}

u32 rtw_rust_sta_info_size(void)
{
	return (u32)sizeof(struct sta_info);
}

u8 rtw_rust_stainfo_offset_valid(int offset)
{
	return stainfo_offset_valid(offset) ? 1 : 0;
}

u8 rtw_rust_stctl_reg_s_proto(struct st_ctl_t *st_ctl, u8 idx)
{
	if (idx >= SESSION_TRACKER_REG_ID_NUM)
		return 0;
	return st_ctl->reg[idx].s_proto;
}

st_match_rule rtw_rust_stctl_reg_rule(struct st_ctl_t *st_ctl, u8 idx)
{
	if (idx >= SESSION_TRACKER_REG_ID_NUM)
		return NULL;
	return st_ctl->reg[idx].rule;
}

void rtw_rust_stctl_reg_set(struct st_ctl_t *st_ctl, u8 idx, u8 s_proto, st_match_rule rule)
{
	if (idx >= SESSION_TRACKER_REG_ID_NUM)
		return;
	st_ctl->reg[idx].s_proto = s_proto;
	st_ctl->reg[idx].rule = rule;
}

void rtw_rust_stctl_reg_clear(struct st_ctl_t *st_ctl, u8 idx)
{
	if (idx >= SESSION_TRACKER_REG_ID_NUM)
		return;
	st_ctl->reg[idx].s_proto = 0;
	st_ctl->reg[idx].rule = NULL;
}

u8 rtw_rust_stctl_any_reg(struct st_ctl_t *st_ctl)
{
	int i;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++)
		if (st_ctl->reg[i].s_proto != 0)
			return 1;
	return 0;
}

#endif /* CONFIG_RUST && !HOST_STA_MGT_TEST */
