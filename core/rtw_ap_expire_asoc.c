/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_ASOC_C_

#ifdef HOST_AP_EXPIRE_ASOC_TEST
#include "host_ap_expire_asoc_types.h"
#else
#include <drv_types.h>
#endif

u8 chk_sta_is_alive(struct sta_info *psta);

void rtw_ap_expire_asoc_sta_tick(_adapter *padapter, struct sta_info *psta)
{
	struct sta_priv *pstapriv = &padapter->stapriv;

	if (chk_sta_is_alive(psta) || !psta->expire_to) {
		psta->expire_to = pstapriv->expire_to;
		psta->keep_alive_trycnt = 0;
#ifndef HOST_AP_EXPIRE_ASOC_TEST
#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
		psta->under_exist_checking = 0;
#endif
#endif
	} else {
		psta->expire_to--;
	}
}
