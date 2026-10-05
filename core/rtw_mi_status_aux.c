/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_MI_STATUS_AUX_C_

#ifdef HOST_MI_STATUS_TEST
#include "host_mi_status_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

void rtw_mi_status(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter_to_dvobj(adapter), 0xFF, mstate);
}

void rtw_mi_status_no_self(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter_to_dvobj(adapter),
			       0xFF & ~BIT(adapter->iface_id), mstate);
}

void rtw_mi_status_no_others(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter_to_dvobj(adapter),
			       BIT(adapter->iface_id), mstate);
}

void rtw_mi_status_merge(struct mi_state *d, struct mi_state *a)
{
	d->sta_num += a->sta_num;
	d->ld_sta_num += a->ld_sta_num;
	d->lg_sta_num += a->lg_sta_num;
#ifdef CONFIG_TDLS
	d->ld_tdls_num += a->ld_tdls_num;
#endif
#ifdef CONFIG_AP_MODE
	d->ap_num += a->ap_num;
	d->ld_ap_num += a->ld_ap_num;
#endif
	d->adhoc_num += a->adhoc_num;
	d->ld_adhoc_num += a->ld_adhoc_num;
#ifdef CONFIG_RTW_MESH
	d->mesh_num += a->mesh_num;
	d->ld_mesh_num += a->ld_mesh_num;
#endif
	d->scan_num += a->scan_num;
	d->scan_enter_num += a->scan_enter_num;
	d->uwps_num += a->uwps_num;
#ifdef CONFIG_IOCTL_CFG80211
	#ifdef CONFIG_P2P
	d->roch_num += a->roch_num;
	#endif
	d->mgmt_tx_num += a->mgmt_tx_num;
#endif
}
