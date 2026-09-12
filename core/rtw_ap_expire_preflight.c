/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_EXPIRE_PREFLIGHT_C_

#include <drv_types.h>

void rtw_ap_expire_auth_list(_adapter *padapter);

#ifdef CONFIG_AP_MODE

/**
 * rtw_ap_expire_timeout_preflight - mesh/WDS/MCC gates and auth-list tick.
 * Returns _TRUE to continue expire_timeout_chk, _FALSE to return early.
 */
u8 rtw_ap_expire_timeout_preflight(_adapter *padapter)
{
#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)
	    && check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE)
	) {
		rtw_mesh_path_expire(padapter);

#ifndef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		return _FALSE;
#endif
	}
#endif

#ifdef CONFIG_RTW_WDS
	rtw_wds_path_expire(padapter);
#endif

#ifdef CONFIG_MCC_MODE
	if (rtw_hal_mcc_link_status_chk(padapter, __func__) == _FALSE)
		return _FALSE;
#endif

	rtw_ap_expire_auth_list(padapter);

	return _TRUE;
}

#endif /* CONFIG_AP_MODE */
