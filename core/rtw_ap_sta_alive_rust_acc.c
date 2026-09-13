// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_sta_alive.rs (W3-82 PR3). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_STA_ALIVE) && !defined(HOST_AP_STA_ALIVE_TEST)

u64 rtw_rust_ap_sta_alive_rx_data(struct sta_info *sta)
{
	return sta->sta_stats.rx_data_pkts;
}

u64 rtw_rust_ap_sta_alive_last_rx_data(struct sta_info *sta)
{
	return sta->sta_stats.last_rx_data_pkts;
}

u64 rtw_rust_ap_sta_alive_rx_ctrl(struct sta_info *sta)
{
	return sta->sta_stats.rx_ctrl_pkts;
}

u64 rtw_rust_ap_sta_alive_last_rx_ctrl(struct sta_info *sta)
{
	return sta->sta_stats.last_rx_ctrl_pkts;
}

void rtw_rust_ap_sta_alive_update_last_rx(struct sta_info *sta)
{
	sta_update_last_rx_pkts(sta);
}

#ifdef CONFIG_RTW_MESH
_adapter *rtw_rust_ap_sta_alive_adapter(struct sta_info *sta)
{
	return sta->padapter;
}

u64 rtw_rust_ap_sta_alive_rx_hwmp(struct sta_info *sta)
{
	return sta->sta_stats.rx_hwmp_pkts;
}

u64 rtw_rust_ap_sta_alive_last_rx_hwmp(struct sta_info *sta)
{
	return sta->sta_stats.last_rx_hwmp_pkts;
}

u64 rtw_rust_ap_sta_alive_rx_beacon(struct sta_info *sta)
{
	return sta->sta_stats.rx_beacon_pkts;
}

u64 rtw_rust_ap_sta_alive_last_rx_beacon(struct sta_info *sta)
{
	return sta->sta_stats.last_rx_beacon_pkts;
}

void rtw_rust_ap_sta_alive_set_alive(struct sta_info *sta, u8 alive)
{
	sta->alive = alive;
}
#endif /* CONFIG_RTW_MESH */

#endif /* CONFIG_RUST_AP_STA_ALIVE && !HOST_AP_STA_ALIVE_TEST */
