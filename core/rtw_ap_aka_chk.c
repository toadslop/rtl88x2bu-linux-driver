/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_AKA_CHK_C_

#ifdef HOST_AP_AKA_CHK_TEST
#include "host_ap_aka_chk_types.h"
#else
#include <drv_types.h>
#endif

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK

/**
 * issue_aka_chk_frame - issue active keep alive check frame
 *	aka = active keep alive
 */
int issue_aka_chk_frame(_adapter *adapter, struct sta_info *psta)
{
	int ret = _FAIL;
	u8 *target_addr = psta->cmn.mac_addr;

	if (MLME_IS_AP(adapter)) {
		/* issue null data to check sta alive */
		if (psta->state & WIFI_SLEEP_STATE)
			ret = issue_nulldata(adapter, target_addr, 0, 1, 50);
		else
			ret = issue_nulldata(adapter, target_addr, 0, 3, 50);
	}

#ifndef HOST_AP_AKA_CHK_TEST
#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		struct rtw_mesh_path *mpath;

		rtw_rcu_read_lock();
		mpath = rtw_mesh_path_lookup(adapter, target_addr);
		if (!mpath) {
			mpath = rtw_mesh_path_add(adapter, target_addr);
			if (IS_ERR(mpath)) {
				rtw_rcu_read_unlock();
				RTW_ERR(FUNC_ADPT_FMT" rtw_mesh_path_add for "MAC_FMT" fail.\n",
					FUNC_ADPT_ARG(adapter), MAC_ARG(target_addr));
				return _FAIL;
			}
		}
		if (mpath->flags & RTW_MESH_PATH_ACTIVE)
			ret = _SUCCESS;
		else {
			u8 flags = RTW_PREQ_Q_F_START | RTW_PREQ_Q_F_PEER_AKA;
			/* issue PREQ to check peer alive */
			rtw_mesh_queue_preq(mpath, flags);
			ret = _FALSE;
		}
		rtw_rcu_read_unlock();
	}
#endif
#endif /* !HOST_AP_AKA_CHK_TEST */
	return ret;
}

#endif /* CONFIG_ACTIVE_KEEP_ALIVE_CHECK */
