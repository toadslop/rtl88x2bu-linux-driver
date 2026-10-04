/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_MI_STATUS_C_

#ifdef HOST_MI_STATUS_TEST
#include "host_mi_status_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

void rtw_mi_status_by_ifbmp(struct dvobj_priv *dvobj, u8 ifbmp, struct mi_state *mstate)
{
	_adapter *iface;
	int i;

	_rtw_memset(mstate, 0, sizeof(struct mi_state));

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;

		if (check_fwstate(&iface->mlmepriv, WIFI_STATION_STATE) == _TRUE) {
			MSTATE_STA_NUM(mstate)++;
			if (check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
				MSTATE_STA_LD_NUM(mstate)++;

				#ifdef CONFIG_TDLS
				if (iface->tdlsinfo.link_established == _TRUE)
					MSTATE_TDLS_LD_NUM(mstate)++;
				#endif
				#ifdef CONFIG_P2P
				if (MLME_IS_GC(iface))
					MSTATE_P2P_GC_NUM(mstate)++;
				#endif
			}
			if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_LINKING) == _TRUE)
				MSTATE_STA_LG_NUM(mstate)++;

#ifdef CONFIG_AP_MODE
		} else if (check_fwstate(&iface->mlmepriv, WIFI_AP_STATE) == _TRUE ) {
			if (check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
				MSTATE_AP_NUM(mstate)++;
				if (iface->stapriv.asoc_sta_count > 2)
					MSTATE_AP_LD_NUM(mstate)++;
				#ifdef CONFIG_P2P
				if (MLME_IS_GO(iface))
					MSTATE_P2P_GO_NUM(mstate)++;
				#endif
			} else
				MSTATE_AP_STARTING_NUM(mstate)++;
#endif

		} else if (check_fwstate(&iface->mlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) == _TRUE
			&& check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE
		) {
			MSTATE_ADHOC_NUM(mstate)++;
			if (iface->stapriv.asoc_sta_count > 2)
				MSTATE_ADHOC_LD_NUM(mstate)++;

#ifdef CONFIG_RTW_MESH
		} else if (check_fwstate(&iface->mlmepriv, WIFI_MESH_STATE) == _TRUE
			&& check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE
		) {
			MSTATE_MESH_NUM(mstate)++;
			if (iface->stapriv.asoc_sta_count > 2)
				MSTATE_MESH_LD_NUM(mstate)++;
#endif

		}

		if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_WPS) == _TRUE)
			MSTATE_WPS_NUM(mstate)++;

		if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_SURVEY) == _TRUE) {
			MSTATE_SCAN_NUM(mstate)++;

			if (mlmeext_scan_state(&iface->mlmeextpriv) != SCAN_DISABLE
				&& mlmeext_scan_state(&iface->mlmeextpriv) != SCAN_BACK_OP)
				MSTATE_SCAN_ENTER_NUM(mstate)++;
		}

#ifdef CONFIG_IOCTL_CFG80211
		if (rtw_cfg80211_get_is_mgmt_tx(iface))
			MSTATE_MGMT_TX_NUM(mstate)++;

		if (rtw_cfg80211_get_is_roch(iface) == _TRUE)
			MSTATE_ROCH_NUM(mstate)++;

#endif /* CONFIG_IOCTL_CFG80211 */
#ifdef CONFIG_P2P
		if (MLME_IS_PD(iface))
			MSTATE_P2P_DV_NUM(mstate)++;
#endif
	}
}
