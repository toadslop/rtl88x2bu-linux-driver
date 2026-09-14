// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_aka_chk.rs (W3-82 PR12). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_AKA_CHK) && defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) \
	&& !defined(HOST_AP_AKA_CHK_TEST)

u8 rtw_rust_aka_mlme_is_ap(_adapter *adapter)
{
	return MLME_IS_AP(adapter) ? _TRUE : _FALSE;
}

u8 rtw_rust_aka_sta_sleep(struct sta_info *psta)
{
	return (psta->state & WIFI_SLEEP_STATE) ? _TRUE : _FALSE;
}

u8 *rtw_rust_aka_sta_mac(struct sta_info *psta)
{
	return psta->cmn.mac_addr;
}

int rtw_rust_aka_chk_mesh(_adapter *adapter, u8 *target_addr, int ap_ret)
{
	int ret = ap_ret;

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

			rtw_mesh_queue_preq(mpath, flags);
			ret = _FALSE;
		}
		rtw_rcu_read_unlock();
	}
#endif
	return ret;
}

#endif /* CONFIG_RUST_AP_AKA_CHK && CONFIG_ACTIVE_KEEP_ALIVE_CHECK && !HOST_AP_AKA_CHK_TEST */
