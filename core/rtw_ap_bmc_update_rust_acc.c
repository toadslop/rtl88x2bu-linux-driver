// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_bmc_update.rs (W3-80 PR10). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_BMC_UPDATE) && !defined(HOST_AP_BMC_UPDATE_TEST)

u8 rtw_rust_bmc_update_bmc_tx_rate(_adapter *adapter)
{
	return adapter->bmc_tx_rate;
}

int rtw_rust_bmc_update_asoc_sta_count(_adapter *adapter)
{
	return adapter->stapriv.asoc_sta_count;
}

void rtw_rust_bmc_update_set_init_rate(struct sta_info *psta, u8 rate)
{
	psta->init_rate = rate;
}

u8 rtw_rust_bmc_update_mlme_is_ap(_adapter *adapter)
{
	return MLME_IS_AP(adapter) ? 1 : 0;
}

u8 rtw_rust_bmc_update_mlme_is_mesh(_adapter *adapter)
{
#ifdef CONFIG_RTW_MESH
	return MLME_IS_MESH(adapter) ? 1 : 0;
#else
	(void)adapter;
	return 0;
#endif
}

u32 rtw_rust_bmc_update_wireless_mode(_adapter *adapter)
{
	return adapter->mlmeextpriv.cur_wireless_mode;
}

u64 rtw_rust_bmc_update_sta_ramask(struct sta_info *psta)
{
	return psta->cmn.ra_info.ramask;
}

#endif /* CONFIG_RUST_AP_BMC_UPDATE && !HOST_AP_BMC_UPDATE_TEST */
