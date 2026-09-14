// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-79 sta priv / mfree / bcmc L2 oracle. */

#include <stdlib.h>
#include <string.h>

#define HOST_STA_MGT_LOCK_SHIM_EXPORT 1
#include "host_sta_mgt_types.h"

void *rtw_zvmalloc(u32 sz)
{
	return calloc(1, sz);
}

void rtw_vmfree(u8 *p, u32 sz)
{
	(void)sz;
	free(p);
}

void *rtw_zmalloc(u32 sz)
{
	return calloc(1, sz);
}

struct macid_ctl_t *adapter_to_macidctl(_adapter *adapter)
{
	return &adapter->macid_ctl;
}

void rtw_macaddr_acl_init(_adapter *adapter, int index)
{
	(void)adapter;
	(void)index;
}

void rtw_macaddr_acl_deinit(_adapter *adapter, int index)
{
	(void)adapter;
	(void)index;
}

void rtw_pre_link_sta_ctl_init(struct sta_priv *stapriv)
{
	(void)stapriv;
}

void rtw_pre_link_sta_ctl_deinit(struct sta_priv *stapriv)
{
	(void)stapriv;
}

void rtw_set_rx_chk_limit(_adapter *adapter, int limit)
{
	(void)adapter;
	(void)limit;
}

void _cancel_timer_ex(void *timer)
{
	(void)timer;
}

void host_sta_mgt_free_reset(_adapter *adapter)
{
	memset(&adapter->stapriv, 0, sizeof(adapter->stapriv));
	memset(&adapter->macid_ctl, 0, sizeof(adapter->macid_ctl));
	adapter->macid_ctl.num = NUM_STA;
	adapter->stapriv.padapter = adapter;
}

int host_sta_mgt_free_setup(_adapter *adapter)
{
	int i;

	host_sta_mgt_free_reset(adapter);
	_rtw_init_queue(&adapter->stapriv.free_sta_queue);
	_rtw_spinlock_init(&adapter->stapriv.sta_hash_lock);
#ifdef CONFIG_AP_MODE
	_rtw_spinlock_init(&adapter->stapriv.auth_list_lock);
#endif
	for (i = 0; i < NUM_STA; i++)
		_rtw_init_listhead(&adapter->stapriv.sta_hash[i]);
	return 0;
}

void rtw_mi_update_iface_status(struct mlme_priv *pmlmepriv, u8 flags)
{
	(void)pmlmepriv;
	(void)flags;
}

void rtw_hal_set_odm_var(_adapter *padapter, int var, struct sta_info *psta,
			 u8 val)
{
	(void)padapter;
	(void)var;
	(void)psta;
	(void)val;
}

void rtw_release_macid(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
}

void rtw_tim_map_clear(_adapter *padapter, u8 *map, u16 aid)
{
	(void)padapter;
	(void)map;
	(void)aid;
}

void rtw_free_stainfo_flush_xmit(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
}

void rtw_free_stainfo_flush_recv(_adapter *padapter, struct sta_info *psta)
{
	(void)padapter;
	(void)psta;
}

void _enter_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

void _exit_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

#ifndef RUST_STA_MGT_FREE_STAINFO_ORACLE
u32 rtw_free_stainfo(_adapter *padapter, struct sta_info *psta)
{
	_irqL irqL0;
	struct sta_priv *pstapriv = &padapter->stapriv;

	if (psta == NULL)
		return _SUCCESS;

	_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
	rtw_list_delete(&psta->hash_list);
	pstapriv->asoc_sta_count--;
	_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
	rtw_mi_update_iface_status(&(padapter->mlmepriv), 0);

	_enter_critical_bh(&psta->lock, &irqL0);
	psta->state &= ~WIFI_ASOC_STATE;
	_exit_critical_bh(&psta->lock, &irqL0);

	rtw_free_stainfo_flush_xmit(padapter, psta);
	rtw_free_stainfo_flush_recv(padapter, psta);

	if (!((psta->state & WIFI_AP_STATE) || MacAddr_isBcst(psta->cmn.mac_addr)))
		rtw_hal_set_odm_var(padapter, 0, psta, _FALSE);

	rtw_release_macid(pstapriv->padapter, psta);
	rtw_st_ctl_deinit(&psta->st_ctl);

	_rtw_spinlock_free(&psta->lock);
	_enter_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);
	rtw_list_insert_tail(&psta->list, get_list_head(&pstapriv->free_sta_queue));
	_exit_critical_bh(&(pstapriv->sta_hash_lock), &irqL0);

	return _SUCCESS;
}
#endif /* !RUST_STA_MGT_FREE_STAINFO_ORACLE */
