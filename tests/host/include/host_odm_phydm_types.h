// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_ODM_PHYDM_TYPES_H
#define HOST_ODM_PHYDM_TYPES_H
#include "host_types.h"

typedef enum {
	HAL_PHYDM_DIS_ALL_FUNC,
	HAL_PHYDM_FUNC_SET,
	HAL_PHYDM_FUNC_CLR,
	HAL_PHYDM_ABILITY_BK,
	HAL_PHYDM_ABILITY_RESTORE,
	HAL_PHYDM_ABILITY_SET,
	HAL_PHYDM_ABILITY_GET,
} HAL_PHYDM_OPS;

#define DYNAMIC_FUNC_DISABLE 0
#define HALRF_CMNINFO_ABILITY 0
#define ODM_CMNINFO_IC_TYPE 0
#define ODM_RTL8822B (1u << 7)

struct dm_struct {
	u64 support_ability;
	u32 bk_support_ability;
};

typedef struct {
	struct dm_struct odmpriv;
	u64 bk_rf_ability;
} HAL_DATA_TYPE;

typedef struct {
	HAL_DATA_TYPE HalData;
	u8 chip_type;
} _adapter;

void rtw_warn_on(int cond);
#define GET_HAL_DATA(a) (&((a)->HalData))
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)
static inline u8 rtw_get_chip_type(_adapter *a) { return a->chip_type; }

u32 chip_type_to_odm_ic_type(u8 chip_type);
void halrf_cmn_info_set(struct dm_struct *dm, u32 cmn_info, u64 value);
u64 halrf_cmn_info_get(struct dm_struct *dm, u32 cmn_info);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u32 value);
u32 host_odm_ic_type(struct dm_struct *dm);
u32 host_odm_rf_ability(struct dm_struct *dm);
void host_odm_reset(_adapter *adapter);
u32 rtw_phydm_ability_ops(_adapter *adapter, HAL_PHYDM_OPS ops, u32 ability);
void rtw_odm_init_ic_type(_adapter *adapter);
#endif
