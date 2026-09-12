/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_TIMEOUT_C_

#ifdef HOST_AP_EXPIRE_TIMEOUT_TEST
#include "host_ap_expire_timeout_types.h"
#else
#include <drv_types.h>
#endif

u8 rtw_ap_expire_timeout_preflight(_adapter *padapter);
void rtw_ap_expire_asoc_list_scan(_adapter *padapter, char *chk_alive_list, u8 *chk_alive_num);
u8 rtw_ap_expire_chk_alive_process(_adapter *padapter, char *chk_alive_list, u8 chk_alive_num);

#ifdef RTW_CONFIG_RFREG18_WA
void rtw_check_restore_rf18(_adapter *padapter);
#endif

#if !defined(CONFIG_RUST_AP_EXPIRE_TIMEOUT) || defined(HOST_AP_EXPIRE_TIMEOUT_TEST)

void expire_timeout_chk(_adapter *padapter)
{
	u8 updated = _FALSE;
	u8 chk_alive_num = 0;
	char chk_alive_list[NUM_STA];

	if (!rtw_ap_expire_timeout_preflight(padapter))
		return;

	rtw_ap_expire_asoc_list_scan(padapter, chk_alive_list, &chk_alive_num);

	if (chk_alive_num)
		updated |= rtw_ap_expire_chk_alive_process(padapter, chk_alive_list, chk_alive_num);

#ifdef RTW_CONFIG_RFREG18_WA
	rtw_check_restore_rf18(padapter);
#endif
	associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
}

#endif /* !CONFIG_RUST_AP_EXPIRE_TIMEOUT || HOST_AP_EXPIRE_TIMEOUT_TEST */
