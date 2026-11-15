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

u8 rtw_rust_hal_dm_reg_iqk_fw_offload(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return GET_HAL_DATA(adapter)->RegIQKFWOffload;
}

void *rtw_rust_hal_dm_iqk_sctx_ptr(_adapter *adapter)
{
	if (!adapter)
		return NULL;
	return &GET_HAL_DATA(adapter)->iqk_sctx;
}

#ifdef CONFIG_FW_OFFLOAD_PARAM_INIT
u8 rtw_rust_hal_dm_fw_param_init(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return adapter->registrypriv.fw_param_init;
}
#endif

u8 rtw_rust_hal_dm_mi_linked(_adapter *adapter)
{
	if (!adapter)
		return 0;
	return rtw_mi_check_status(adapter, MI_LINKED);
}

void rtw_rust_hal_dm_lps_leave_iqk_switch(_adapter *adapter)
{
#ifdef CONFIG_LPS
	if (adapter)
		LPS_Leave(adapter, "SWITCH_IQK_OFFLOAD");
#endif
}

void rtw_rust_hal_dm_log_iqk_fw_offload(u8 enable)
{
	RTW_INFO("IQK FW offload:%s\n", enable ? "enable" : "disable");
}

void rtw_rust_hal_dm_log_param_init_fw_offload(u8 enable)
{
	RTW_INFO("Init-Parameter FW offload:%s\n", enable ? "enable" : "disable");
}

u16 rtw_rust_hal_dm_phydm_rf_iqk_offload(void)
{
	return PHYDM_RF_IQK_OFFLOAD;
}

u16 rtw_rust_hal_dm_phydm_phy_param_offload(void)
{
	return PHYDM_PHY_PARAM_OFFLOAD;
}

struct rtw_hal_dm_init_odm_mid_init {
	u16 type_gpa;
	u16 type_apa;
	u16 type_glna;
	u16 type_alna;
	u16 rfe_type;
	u8 crystal_cap;
	u8 reg_pwr_trim_enable;
	u8 rf_kfree_enable;
	u8 trx_ant_div_type;
	u8 b_fix_tx_ant;
	u8 with_ext_ant_switch;
	u8 efuse0x3d7;
	u8 efuse0x3d8;
	u8 eeprom_thermal_meter;
	u8 txpwr_pg_mode;
	u8 adaptivity_mode;
	s8 adaptivity_th_l2h_ini;
	s8 adaptivity_th_edcca_hl_diff;
};

_Static_assert(sizeof(struct rtw_hal_dm_init_odm_mid_init) == 24,
	"rtw_hal_dm_init_odm_mid_init must match Rust RtwHalDmInitOdmMidInit layout");
_Static_assert(sizeof_field(struct rtw_hal_dm_init_odm_mid_init, type_gpa) == 2,
	"type_gpa must be u16");

void rtw_rust_hal_dm_init_odm_mid_init_vals(_adapter *adapter,
	struct rtw_hal_dm_init_odm_mid_init *out)
{
	PHAL_DATA_TYPE hal;

	if (!adapter || !out)
		return;
	hal = GET_HAL_DATA(adapter);
	out->type_gpa = hal->TypeGPA;
	out->type_apa = hal->TypeAPA;
	out->type_glna = hal->TypeGLNA;
	out->type_alna = hal->TypeALNA;
	out->rfe_type = hal->rfe_type;
	out->crystal_cap = hal->crystal_cap;
	out->reg_pwr_trim_enable = adapter->registrypriv.RegPwrTrimEnable;
	out->rf_kfree_enable = hal->RfKFreeEnable;
	out->trx_ant_div_type = hal->TRxAntDivType;
	out->b_fix_tx_ant = hal->b_fix_tx_ant;
	out->with_ext_ant_switch = hal->with_extenal_ant_switch;
	out->efuse0x3d7 = hal->efuse0x3d7;
	out->efuse0x3d8 = hal->efuse0x3d8;
	out->eeprom_thermal_meter = hal->eeprom_thermal_meter;
	out->txpwr_pg_mode = hal->txpwr_pg_mode;
	out->adaptivity_mode = adapter->registrypriv.adaptivity_mode;
	out->adaptivity_th_l2h_ini = adapter->registrypriv.adaptivity_th_l2h_ini;
	out->adaptivity_th_edcca_hl_diff = adapter->registrypriv.adaptivity_th_edcca_hl_diff;
}

u32 *rtw_rust_hal_dm_init_odm_mid_hook_phydm_ability(_adapter *adapter)
{
	if (!adapter)
		return NULL;
	return &(adapter->registrypriv.phydm_ability);
}

u8 *rtw_rust_hal_dm_init_odm_mid_hook_adaptivity_en(_adapter *adapter)
{
	if (!adapter)
		return NULL;
	return &(adapter->registrypriv.adaptivity_en);
}

void rtw_rust_hal_dm_init_odm_iqk_pa_off(struct dm_struct *phydm)
{
#ifdef CONFIG_IQK_PA_OFF
	if (phydm)
		odm_cmn_info_init(phydm, ODM_CMNINFO_IQKPAOFF, 1);
#else
	(void)phydm;
#endif
}

void rtw_rust_hal_dm_init_odm_param_offload_cap(_adapter *adapter)
{
#ifdef CONFIG_FW_OFFLOAD_PARAM_INIT
	if (adapter)
		rtw_hal_update_param_init_fw_offload_cap(adapter);
#else
	(void)adapter;
#endif
}

struct rtw_hal_dm_init_odm_tail_hooks {
	u8 *current_channel;
	u8 *b_scan_in_process;
	u8 *bpower_saving;
	u32 *cur_tx_tp;
	u32 *cur_rx_tp;
	u8 *antenna_test;
	u8 *multi_ch_switch_mode;
	u32 *halrf_ability;
};

_Static_assert(sizeof(struct rtw_hal_dm_init_odm_tail_hooks) == 8 * sizeof(void *),
	"rtw_hal_dm_init_odm_tail_hooks layout must match Rust RtwHalDmInitOdmTailHooks");

void rtw_rust_hal_dm_init_odm_tail_fill_hooks(_adapter *adapter,
	struct rtw_hal_dm_init_odm_tail_hooks *out)
{
	struct dvobj_priv *dvobj;
	PHAL_DATA_TYPE hal;
	struct pwrctrl_priv *pwrctl;

	if (!adapter || !out)
		return;
	dvobj = adapter_to_dvobj(adapter);
	hal = GET_HAL_DATA(adapter);
	pwrctl = adapter_to_pwrctl(adapter);
	out->current_channel = &(hal->current_channel);
	out->b_scan_in_process = &(hal->bScanInProcess);
	out->bpower_saving = &(pwrctl->bpower_saving);
	out->cur_tx_tp = &(dvobj->traffic_stat.cur_tx_tp);
	out->cur_rx_tp = &(dvobj->traffic_stat.cur_rx_tp);
	out->antenna_test = &(hal->antenna_test);
	out->multi_ch_switch_mode = &(hal->multi_ch_switch_mode);
	out->halrf_ability = &(adapter->registrypriv.halrf_ability);
}

void rtw_rust_hal_dm_init_odm_tail_optional_hooks(_adapter *adapter, struct dm_struct *phydm)
{
	struct dvobj_priv *dvobj;
	PHAL_DATA_TYPE hal;

	if (!adapter || !phydm)
		return;
	dvobj = adapter_to_dvobj(adapter);
	hal = GET_HAL_DATA(adapter);
#ifdef CONFIG_RTL8723B
	odm_cmn_info_hook(phydm, ODM_CMNINFO_IS1ANTENNA, &hal->EEPROMBluetoothAntNum);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_RFDEFAULTPATH, &hal->ant_path);
#endif
#ifdef CONFIG_USB_HCI
	odm_cmn_info_hook(phydm, ODM_CMNINFO_HUBUSBMODE, &(dvobj->usb_speed));
#endif
#ifdef CONFIG_DYNAMIC_SOML
	odm_cmn_info_hook(phydm, ODM_CMNINFO_ADAPTIVE_SOML, &(adapter->registrypriv.dyn_soml_en));
#endif
#ifdef CONFIG_RTW_PATH_DIV
	odm_cmn_info_hook(phydm, ODM_CMNINFO_PATH_DIV, &(adapter->registrypriv.path_div));
#endif
#ifdef CONFIG_MP_INCLUDED
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_CON_TX, &(adapter->mppriv.mpt_ctx.is_start_cont_tx));
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_SINGLE_TONE, &(adapter->mppriv.mpt_ctx.is_single_tone));
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_CARRIER_SUPPRESSION, &(adapter->mppriv.mpt_ctx.is_carrier_suppression));
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_MP_RATE_INDEX, &(adapter->mppriv.mpt_ctx.mpt_rate_index));
#endif
}

void rtw_rust_hal_dm_init_odm_tail_sta_hooks(struct dm_struct *phydm)
{
	int i;

	if (!phydm)
		return;
	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		phydm_cmn_sta_info_hook(phydm, i, NULL);
}

void rtw_rust_hal_dm_init_odm_tail_finish(struct dm_struct *phydm)
{
	if (!phydm)
		return;
	rtw_phydm_ops_func_init(phydm);
	phydm_dm_early_init(phydm);
}
#endif /* CONFIG_RUST */
