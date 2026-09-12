// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_expire_asoc.rs (W3-82 PR8). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_EXPIRE_ASOC) && !defined(HOST_AP_EXPIRE_ASOC_TEST)

u8 rtw_rust_expire_asoc_stapriv_expire_to(_adapter *padapter)
{
	return padapter->stapriv.expire_to;
}

u8 rtw_rust_expire_asoc_sta_expire_to(struct sta_info *psta)
{
	return psta->expire_to;
}

void rtw_rust_expire_asoc_sta_set_expire_to(struct sta_info *psta, u8 v)
{
	psta->expire_to = v;
}

void rtw_rust_expire_asoc_sta_set_keep_alive_trycnt(struct sta_info *psta, u8 v)
{
	psta->keep_alive_trycnt = v;
}

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
void rtw_rust_expire_asoc_sta_clear_under_exist_checking(struct sta_info *psta)
{
	psta->under_exist_checking = 0;
}
#endif

#endif /* CONFIG_RUST_AP_EXPIRE_ASOC && !HOST_AP_EXPIRE_ASOC_TEST */
