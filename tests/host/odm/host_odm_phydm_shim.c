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

