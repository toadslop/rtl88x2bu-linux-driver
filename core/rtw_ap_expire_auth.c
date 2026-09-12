/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_AUTH_C_

#ifdef HOST_AP_EXPIRE_AUTH_TEST
#include "host_ap_expire_auth_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST_AP_EXPIRE_AUTH) || defined(HOST_AP_EXPIRE_AUTH_TEST)

void rtw_ap_expire_auth_list(_adapter *padapter)
{
	_irqL irqL;
	_list *phead, *plist;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	int i;
	int stainfo_offset;
	u8 flush_num = 0;
#ifdef HOST_AP_EXPIRE_AUTH_TEST
	char flush_list[HOST_EXPIRE_AUTH_MAX_STA] = {0};
#else
	char flush_list[NUM_STA] = {0};
#endif

#ifdef HOST_AP_EXPIRE_AUTH_TEST
	host_expire_auth_reset_flush_count();
#endif

	_enter_critical_bh(&pstapriv->auth_list_lock, &irqL);

	phead = &pstapriv->auth_list;
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, auth_list);

		plist = get_next(plist);

#ifdef CONFIG_ATMEL_RC_PATCH
		if (_rtw_memcmp((void *)(pstapriv->atmel_rc_pattern), (void *)(psta->cmn.mac_addr), ETH_ALEN) == _TRUE)
			continue;
		if (psta->flag_atmel_rc)
			continue;
#endif
		if (psta->expire_to > 0) {
			psta->expire_to--;
			if (psta->expire_to == 0) {
				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					flush_list[flush_num++] = (char)stainfo_offset;
#ifndef HOST_AP_EXPIRE_AUTH_TEST
				else
					rtw_warn_on(1);
#endif
			}
		}
	}

	_exit_critical_bh(&pstapriv->auth_list_lock, &irqL);
	for (i = 0; i < flush_num; i++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, flush_list[i]);
#ifndef HOST_AP_EXPIRE_AUTH_TEST
		RTW_INFO(FUNC_ADPT_FMT" auth expire "MAC_FMT"\n"
			, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->cmn.mac_addr));
#endif
		rtw_free_stainfo(padapter, psta);
		psta = NULL;
	}
}

#endif /* !CONFIG_RUST_AP_EXPIRE_AUTH || HOST_AP_EXPIRE_AUTH_TEST */
