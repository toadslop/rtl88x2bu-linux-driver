// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_expire_auth.rs (W3-82 PR9). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_EXPIRE_AUTH) && !defined(HOST_AP_EXPIRE_AUTH_TEST)

void rtw_rust_expire_auth_lock(struct sta_priv *pstapriv, _irqL *irqL)
{
	_enter_critical_bh(&pstapriv->auth_list_lock, irqL);
}

void rtw_rust_expire_auth_unlock(struct sta_priv *pstapriv, _irqL *irqL)
{
	_exit_critical_bh(&pstapriv->auth_list_lock, irqL);
}

struct sta_priv *rtw_rust_expire_auth_stapriv(_adapter *padapter)
{
	return &padapter->stapriv;
}

_list *rtw_rust_expire_auth_auth_head(struct sta_priv *pstapriv)
{
	return &pstapriv->auth_list;
}

_list *rtw_rust_expire_auth_list_next(_list *list)
{
	return get_next(list);
}

u8 rtw_rust_expire_auth_queue_end(_list *head, _list *elem)
{
	return rtw_end_of_queue_search(head, elem);
}

struct sta_info *rtw_rust_expire_auth_sta_from_list(_list *plist)
{
	return LIST_CONTAINOR(plist, struct sta_info, auth_list);
}

u8 rtw_rust_expire_auth_sta_expire_to(struct sta_info *psta)
{
	return psta->expire_to;
}

void rtw_rust_expire_auth_sta_dec_expire_to(struct sta_info *psta)
{
	psta->expire_to--;
}

u8 rtw_rust_expire_auth_stainfo_offset_valid(int offset)
{
	return stainfo_offset_valid(offset);
}

void rtw_rust_expire_auth_warn_invalid_offset(void)
{
	rtw_warn_on(1);
}

#endif /* CONFIG_RUST_AP_EXPIRE_AUTH && !HOST_AP_EXPIRE_AUTH_TEST */
