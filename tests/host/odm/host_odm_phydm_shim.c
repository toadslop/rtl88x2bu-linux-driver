// SPDX-License-Identifier: GPL-2.0
#include "host_odm_phydm_types.h"

static u64 g_rf_ability;
static u32 g_ic_type;

/* Same table as _chip_type_to_odm_ic_type[] in hal/hal_intf.c */
static const u32 chip_map[] = {
	0,
	ODM_RTL8188E,
	ODM_RTL8192E,
	ODM_RTL8812,
	ODM_RTL8821,
	ODM_RTL8723B,
	ODM_RTL8814A,
	ODM_RTL8703B,
	ODM_RTL8188F,
	ODM_RTL8188F,
	ODM_RTL8822B,
	ODM_RTL8723D,
	ODM_RTL8821C,
	ODM_RTL8710B,
	ODM_RTL8192F,
	ODM_RTL8822C,
	ODM_RTL8814B,
	ODM_RTL8723F,
	0,
};

u32 chip_type_to_odm_ic_type(u8 chip_type)
{
	return chip_type < (sizeof(chip_map) / sizeof(chip_map[0])) ? chip_map[chip_type] : 0;
}

void halrf_cmn_info_set(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	(void)dm;
	if (cmn_info == HALRF_CMNINFO_ABILITY)
		g_rf_ability = value;
}

u64 halrf_cmn_info_get(struct dm_struct *dm, u32 cmn_info)
{
	(void)dm;
	return cmn_info == HALRF_CMNINFO_ABILITY ? g_rf_ability : 0;
}

void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u32 value)
{
	(void)dm;
	if (cmn_info == ODM_CMNINFO_IC_TYPE)
		g_ic_type = value;
}

u32 host_odm_ic_type(struct dm_struct *dm)
{
	(void)dm;
	return g_ic_type;
}

u32 host_odm_rf_ability(struct dm_struct *dm)
{
	(void)dm;
	return (u32)g_rf_ability;
}

void host_odm_reset(_adapter *adapter)
{
	(void)adapter;
	g_rf_ability = 0;
	g_ic_type = 0;
}

void rtw_warn_on(int cond)
{
	(void)cond;
}

/* Host Rust oracle linkage (mirrors kernel shims in core/rtw_odm_phydm_init.c). */
struct dm_struct *rtw_rust_odm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

u64 *rtw_rust_odm_support_ability(struct dm_struct *dm)
{
	return &dm->support_ability;
}

u32 *rtw_rust_odm_bk_support_ability(struct dm_struct *dm)
{
	return &dm->bk_support_ability;
}

u64 *rtw_rust_odm_bk_rf_ability(_adapter *adapter)
{
	return &GET_HAL_DATA(adapter)->bk_rf_ability;
}

void rtw_rust_odm_halrf_cmn_info_set(struct dm_struct *dm, u64 value)
{
	halrf_cmn_info_set(dm, HALRF_CMNINFO_ABILITY, value);
}

u64 rtw_rust_odm_halrf_cmn_info_get(struct dm_struct *dm)
{
	return halrf_cmn_info_get(dm, HALRF_CMNINFO_ABILITY);
}

void rtw_rust_odm_odm_cmn_info_init(struct dm_struct *dm, u32 ic_type)
{
	odm_cmn_info_init(dm, ODM_CMNINFO_IC_TYPE, ic_type);
}

u32 rtw_rust_odm_chip_type_to_ic(u8 chip_type)
{
	return chip_type_to_odm_ic_type(chip_type);
}

u8 rtw_rust_odm_get_chip_type(_adapter *adapter)
{
	return rtw_get_chip_type(adapter);
}

void rtw_rust_odm_warn_on(int cond)
{
	rtw_warn_on(cond);
}

/* Unused by phydm-init L2 vectors; satisfy rust/rtw_odm.rs link when built as staticlib. */
static s8 stub_adaptivity_th_l2h;
static s8 stub_adaptivity_th_edcca;

void rtw_rust_odm_adaptivity_print_sel(void *sel, const char *line)
{
	(void)sel;
	(void)line;
}

struct dm_struct *rtw_rust_odm_adaptivity_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

u8 rtw_rust_odm_adaptivity_en(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_odm_adaptivity_mode(_adapter *adapter)
{
	(void)adapter;
	return 0;
}

s8 *rtw_rust_odm_adaptivity_th_l2h_ini(struct dm_struct *dm)
{
	(void)dm;
	return &stub_adaptivity_th_l2h;
}

s8 *rtw_rust_odm_adaptivity_th_edcca_hl(struct dm_struct *dm)
{
	(void)dm;
	return &stub_adaptivity_th_edcca;
}

u8 rtw_rust_odm_adaptivity_rx_rate(struct dm_struct *dm)
{
	(void)dm;
	return 0;
}

u8 rtw_rust_odm_adaptivity_rssi_a(struct dm_struct *dm)
{
	(void)dm;
	return 0;
}

u8 rtw_rust_odm_adaptivity_rssi_b(struct dm_struct *dm)
{
	(void)dm;
	return 0;
}

void rtw_rust_odm_adaptivity_print_parm_line(void *sel, u8 th_l2h, s8 th_edcca_hl)
{
	(void)sel;
	(void)th_l2h;
	(void)th_edcca_hl;
}

void rtw_rust_odm_adaptivity_print_perpkt(void *sel, u8 rate, u8 rssi_a, u8 rssi_b)
{
	(void)sel;
	(void)rate;
	(void)rssi_a;
	(void)rssi_b;
}

