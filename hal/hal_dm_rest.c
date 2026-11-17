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

void rtw_rust_hal_dm_raw_print_sel(void *sel, const char *line)
{
	_RTW_PRINT_SEL(sel, "%s", line);
}

void rtw_rust_hal_dm_dump_rx_dframe_info(_adapter *adapter, void *sel)
{
#ifdef DBG_RX_DFRAME_RAW_DATA
	rtw_dump_rx_dframe_info(adapter, sel);
#else
	(void)adapter;
	(void)sel;
#endif
}

void rtw_rust_hal_dm_dump_raw_rssi_info(_adapter *adapter, void *sel)
{
#ifdef DBG_RX_SIGNAL_DISPLAY_RAW_DATA
	rtw_dump_raw_rssi_info(adapter, sel);
#else
	(void)adapter;
	(void)sel;
#endif
}

struct rtw_hal_dm_dm_rx_fields {
	u8 is_linked;
	u8 rx_rate;
	u8 rssi_a;
	u8 rssi_b;
	u8 rssi_c;
	u8 rssi_d;
};

struct rtw_hal_dm_dm_rx_fields rtw_rust_hal_dm_dm_rx_fields(struct dm_struct *podm)
{
	struct rtw_hal_dm_dm_rx_fields out;

	out.is_linked = podm->is_linked;
	out.rx_rate = podm->rx_rate;
	out.rssi_a = podm->rssi_a;
	out.rssi_b = podm->rssi_b;
	out.rssi_c = podm->rssi_c;
	out.rssi_d = podm->rssi_d;
	return out;
}

_Static_assert(sizeof(struct rtw_hal_dm_dm_rx_fields) == 6,
	"rtw_hal_dm_dm_rx_fields layout must match Rust");

u8 rtw_rust_hal_dm_is_hw_8814a(_adapter *adapter)
{
	return IS_HARDWARE_TYPE_8814A(adapter) ? _TRUE : _FALSE;
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

#ifdef CONFIG_ANTENNA_DIVERSITY
u8 rtw_rust_hal_dm_fat_rx_idle_ant(struct dm_struct *phydm)
{
	if (!phydm)
		return 0;
	return phydm->dm_fat_table.rx_idle_ant;
}
#endif /* CONFIG_ANTENNA_DIVERSITY */

struct macid_ctl_t *rtw_rust_hal_dm_macidctl(_adapter *adapter)
{
	if (!adapter)
		return NULL;
	return adapter_to_macidctl(adapter);
}

struct sta_info *rtw_rust_hal_dm_macid_sta(struct macid_ctl_t *ctl, int i)
{
	if (!ctl || i < 0 || i >= MACID_NUM_SW_LIMIT)
		return NULL;
	return ctl->sta[i];
}

u8 rtw_rust_hal_dm_macid_h2c_opmode(struct macid_ctl_t *ctl, int i)
{
	if (!ctl || i < 0 || i >= MACID_NUM_SW_LIMIT)
		return 0;
	return GET_H2CCMD_MSRRPT_PARM_OPMODE(ctl->h2c_msr + i);
}

s8 rtw_rust_hal_dm_sta_rssi(struct sta_info *sta)
{
	if (!sta)
		return 0;
	return sta->cmn.rssi_stat.rssi;
}

u8 rtw_rust_hal_dm_is_broadcast_sta(struct sta_info *sta)
{
	if (!sta)
		return _TRUE;
	return is_broadcast_mac_addr(sta->cmn.mac_addr) ? _TRUE : _FALSE;
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

void rtw_rust_hal_dm_err_iqk_in_progress(void)
{
	RTW_ERR("IQK InProgress\n");
}

u8 rtw_rust_hal_dm_phydm_iqk_in_progress_flag(struct dm_struct *phydm)
{
	if (!phydm)
		return _FALSE;
	return phydm->rf_calibrate_info.is_iqk_in_progress == _TRUE ? _TRUE : _FALSE;
}

#ifdef CONFIG_RUST
u8 rtw_rust_hal_dm_sta_mac_id(struct sta_info *psta)
{
	if (!psta)
		return 0;
	return psta->cmn.mac_id;
}

void rtw_rust_hal_dm_sta_set_dm_ctrl(struct sta_info *psta, u8 dm_ctrl)
{
	if (psta)
		psta->cmn.dm_ctrl = dm_ctrl;
}

void *rtw_rust_hal_dm_sta_cmn_ptr(struct sta_info *psta)
{
	return psta ? &(psta->cmn) : NULL;
}

void rtw_rust_hal_dm_log_set_sta(u8 mac_id)
{
	RTW_INFO("### Set STA_(%d) info ###\n", mac_id);
}

void rtw_rust_hal_dm_log_clean_sta(u8 mac_id)
{
	RTW_INFO("### Clean STA_(%d) info ###\n", mac_id);
}

void rtw_rust_hal_dm_set_odm_var_rest(
	PADAPTER Adapter,
	HAL_ODM_VARIABLE eVariable,
	void *pValue1,
	BOOLEAN bSet)
{
	(void)Adapter;
	(void)bSet;
	switch (eVariable) {
#ifdef CONFIG_ANTENNA_DIVERSITY
	case HAL_ODM_ANTDIV_SELECT: {
		u8 antenna = (*(u8 *)pValue1);
		HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);

		odm_update_rx_idle_ant(&pHalData->odmpriv, antenna);
	}
		break;
#endif

	default:
		break;
	}
}
#endif /* CONFIG_RUST */

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
	u8 *usb_speed;
	u8 *multi_ch_switch_mode;
	u32 *halrf_ability;
};

_Static_assert(sizeof(struct rtw_hal_dm_init_odm_tail_hooks) == 9 * sizeof(void *),
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
	out->usb_speed = &(dvobj->usb_speed);
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

struct rtw_hal_dm_sta_ra_fields {
	u8 ra_bw_mode;
	u8 is_vht_enable;
};

_Static_assert(sizeof(struct rtw_hal_dm_sta_ra_fields) == 2,
	"rtw_hal_dm_sta_ra_fields layout must match Rust RtwHalDmStaRaFields");

void *rtw_rust_hal_dm_phydm_adapter(struct dm_struct *dm)
{
	return dm ? dm->adapter : NULL;
}

void *rtw_rust_hal_dm_ra_table(struct dm_struct *dm)
{
	return dm ? &dm->dm_ra_table : NULL;
}

void rtw_rust_hal_dm_ra_table_set_record_ra(struct ra_table *ra,
	void (*fn)(void *, u8, struct cmn_sta_info *, u64))
{
	if (ra)
		ra->record_ra_info = fn;
}

void rtw_rust_hal_dm_set_fill_desc_dyntxpwr(struct dm_struct *dm,
	void (*fn)(void *, u8 *, u8))
{
	if (dm)
		dm->fill_desc_dyntxpwr = fn;
}

void rtw_rust_hal_dm_sta_ra_fields(struct cmn_sta_info *sta,
	struct rtw_hal_dm_sta_ra_fields *out)
{
	if (!sta || !out)
		return;

	out->ra_bw_mode = (u8)sta->ra_info.ra_bw_mode;
	out->is_vht_enable = sta->ra_info.is_vht_enable;
}

struct dvobj_priv *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter)
{
	return adapter_to_dvobj(adapter);
}

struct macid_ctl_t *rtw_rust_hal_dm_dvobj_to_macidctl(struct dvobj_priv *dvobj)
{
	return dvobj_to_macidctl(dvobj);
}

void rtw_rust_hal_dm_init_odm_tail_finish(struct dm_struct *phydm)
{
	if (!phydm)
		return;
	rtw_phydm_ops_func_init(phydm);
	phydm_dm_early_init(phydm);
}
#endif /* CONFIG_RUST */
