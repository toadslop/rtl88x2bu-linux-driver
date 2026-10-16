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
#endif /* CONFIG_RUST */
