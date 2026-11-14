/******************************************************************************
 *
 * Copyright(c) 2014 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HAL_DM_REST_C_

#include "hal_dm.c"

/* rtw_rust_hal_dm_init_odm_board_type — rust/hal_dm.rs (W4-30) */

#ifdef CONFIG_RUST
struct rtw_hal_dm_sta_dump_fields {
	u8 mac_addr[6];
	u8 mac_id;
	u8 wireless_mode;
	u8 mimo_type;
	u8 sm_ps;
	u8 bw_mode;
	u8 ra_bw_mode;
	u8 rate_id;
	s8 rssi;
	u8 rssi_level;
	u8 is_support_sgi;
	u8 is_vht_enable;
	u8 disable_ra;
	u8 disable_pt;
	u8 is_noisy;
	u8 txrx_state;
	u8 curr_tx_bw;
	u8 curr_retry_ratio;
	u64 ramask;
};

void rtw_rust_hal_dm_print_sel(void *sel, const char *line)
{
	RTW_PRINT_SEL(sel, "%s", line);
}

const char *rtw_rust_hal_dm_ch_width_str(u8 bw)
{
	return ch_width_str(bw);
}

const char *rtw_rust_hal_dm_hdata_rate(u8 rate)
{
	return HDATA_RATE(rate);
}

void *rtw_rust_hal_dm_sta_adapter(struct sta_info *psta)
{
	return psta ? psta->padapter : NULL;
}

u8 rtw_rust_hal_dm_current_tx_rate(_adapter *adapter, struct sta_info *psta)
{
	return rtw_get_current_tx_rate(adapter, psta);
}

u8 rtw_rust_hal_dm_current_tx_sgi(_adapter *adapter, struct sta_info *psta)
{
	return rtw_get_current_tx_sgi(adapter, psta);
}

void rtw_rust_hal_dm_sta_fields(struct sta_info *psta,
	struct rtw_hal_dm_sta_dump_fields *out)
{
	struct ra_sta_info *ra_info;

	if (!psta || !out)
		return;

	ra_info = &psta->cmn.ra_info;
	memset(out, 0, sizeof(*out));
	memcpy(out->mac_addr, psta->cmn.mac_addr, ETH_ALEN);
	out->mac_id = psta->cmn.mac_id;
	out->wireless_mode = psta->wireless_mode;
	out->mimo_type = psta->cmn.mimo_type;
	out->sm_ps = psta->cmn.sm_ps;
	out->bw_mode = psta->cmn.bw_mode;
	out->ra_bw_mode = ra_info->ra_bw_mode;
	out->rate_id = ra_info->rate_id;
	out->rssi = psta->cmn.rssi_stat.rssi;
	out->rssi_level = ra_info->rssi_level;
	out->is_support_sgi = ra_info->is_support_sgi;
	out->is_vht_enable = ra_info->is_vht_enable;
	out->disable_ra = ra_info->disable_ra;
	out->disable_pt = ra_info->disable_pt;
	out->is_noisy = ra_info->is_noisy;
	out->txrx_state = ra_info->txrx_state;
	out->curr_tx_bw = ra_info->curr_tx_bw;
	out->curr_retry_ratio = ra_info->curr_retry_ratio;
	out->ramask = ra_info->ramask;
}

struct rtw_hal_dm_sta_traffic_fields {
	u8 mac_addr[6];
	u8 mac_id;
	s8 rssi;
	u8 curr_rx_rate;
	u8 curr_rx_rate_bmc;
	u32 tx_tp_kbits;
	u32 rx_tp_kbits;
	u32 smooth_tx_tp_kbits;
	u32 smooth_rx_tp_kbits;
};

void rtw_rust_hal_dm_sta_traffic_fields(struct sta_info *psta,
	struct rtw_hal_dm_sta_traffic_fields *out)
{
	if (!psta || !out)
		return;

	memset(out, 0, sizeof(*out));
	memcpy(out->mac_addr, psta->cmn.mac_addr, ETH_ALEN);
	out->mac_id = psta->cmn.mac_id;
	out->rssi = psta->cmn.rssi_stat.rssi;
	out->curr_rx_rate = psta->curr_rx_rate;
	out->curr_rx_rate_bmc = psta->curr_rx_rate_bmc;
	out->tx_tp_kbits = psta->sta_stats.tx_tp_kbits;
	out->rx_tp_kbits = psta->sta_stats.rx_tp_kbits;
	out->smooth_tx_tp_kbits = psta->sta_stats.smooth_tx_tp_kbits;
	out->smooth_rx_tp_kbits = psta->sta_stats.smooth_rx_tp_kbits;
}

int rtw_rust_hal_dm_is_client_associated_to_ap(_adapter *padapter)
{
	return is_client_associated_to_ap(padapter);
}

u8 rtw_rust_hal_dm_get_bcn_cnt(_adapter *adapter)
{
	return rtw_get_bcn_cnt(adapter);
}

u8 rtw_rust_hal_dm_get_bcn_dtim_period(_adapter *adapter)
{
	return rtw_get_bcn_dtim_period(adapter);
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	if (!adapter)
		return NULL;
	return adapter_to_phydm(adapter);
}

void rtw_rust_hal_dm_phydm_set_adapter(struct dm_struct *phydm, _adapter *adapter)
{
	if (phydm)
		phydm->adapter = adapter;
}

void rtw_rust_hal_dm_set_phydm_op_mode(_adapter *adapter, u8 mode)
{
	if (adapter)
		GET_HAL_DATA(adapter)->phydm_op_mode = mode;
}

u8 rtw_rust_hal_dm_mp_test_chip_value(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return IS_NORMAL_CHIP(GET_HAL_DATA(adapter)->version_id);
}

u32 rtw_rust_hal_dm_patch_id(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return GET_HAL_DATA(adapter)->CustomerID;
}

u8 rtw_rust_hal_dm_wifi_spec(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return adapter->registrypriv.wifi_spec;
}

u8 rtw_rust_hal_dm_rf_type(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return GET_HAL_DATA(adapter)->rf_type;
}

u8 rtw_rust_hal_dm_get_intf_type(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return rtw_get_intf_type(adapter);
}

void rtw_rust_hal_dm_init_advance_ota_if_enabled(_adapter *adapter, struct dm_struct *phydm)
{
#ifdef CONFIG_ADVANCE_OTA
	if (adapter && phydm)
		odm_cmn_info_init(phydm, ODM_CMNINFO_ADVANCE_OTA, adapter->registrypriv.adv_ota);
#else
	(void)adapter;
	(void)phydm;
#endif
}

void rtw_rust_hal_dm_hook_dfs_master(_adapter *adapter, struct dm_struct *phydm)
{
#ifdef CONFIG_DFS_MASTER
	struct dvobj_priv *dvobj;

	if (!adapter || !phydm)
		return;
	dvobj = adapter_to_dvobj(adapter);
	rtw_odm_update_dfs_region(dvobj);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_DFS_MASTER_ENABLE,
		&(adapter_to_rfctl(adapter)->radar_detect_enabled));
#else
	(void)adapter;
	(void)phydm;
#endif
}
#endif /* CONFIG_RUST */
