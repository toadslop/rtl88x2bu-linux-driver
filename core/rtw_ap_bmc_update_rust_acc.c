// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_bmc_update.rs (W3-80 PR10). */
#include <drv_types.h>
#include <ieee80211.h>

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

u8 rtw_rust_bmc_update_is_enable_hw_ofdm(_adapter *adapter)
{
	return IsEnableHWOFDM(adapter->mlmeextpriv.cur_wireless_mode) ? 1 : 0;
}

void rtw_rust_bmc_update_err_missing_bmc_sta(_adapter *adapter)
{
	RTW_ERR(ADPT_FMT "could not get bmc_sta !!\n", ADPT_ARG(adapter));
}

u8 *rtw_rust_bmc_update_cur_supported_rates(_adapter *adapter)
{
	WLAN_BSSID_EX *net = &adapter->mlmepriv.cur_network.network;

	return (u8 *)&net->SupportedRates;
}

int rtw_rust_bmc_update_cur_ds_config(_adapter *adapter)
{
	return adapter->mlmepriv.cur_network.network.Configuration.DSConfig;
}

void rtw_rust_bmc_update_sta_prepare(_adapter *adapter, struct sta_info *psta)
{
	psta->cmn.aid = 0;
#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter))
		psta->qos_option = 1;
	else
#endif
		psta->qos_option = 0;
#ifdef CONFIG_80211N_HT
	psta->htpriv.ht_option = _FALSE;
#endif
	psta->ieee8021x_blocked = 0;
	_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));
}

void rtw_rust_bmc_update_sta_set_asoc(struct sta_info *psta)
{
	_irqL irqL;

	_enter_critical_bh(&psta->lock, &irqL);
	psta->state = WIFI_ASOC_STATE;
	_exit_critical_bh(&psta->lock, &irqL);
}

void rtw_rust_bmc_update_sta_set_wireless_mode(struct sta_info *psta, u8 mode)
{
	psta->wireless_mode = mode;
}

#endif /* CONFIG_RUST_AP_BMC_UPDATE && !HOST_AP_BMC_UPDATE_TEST */
