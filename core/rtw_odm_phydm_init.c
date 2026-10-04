// SPDX-License-Identifier: GPL-2.0
#define _RTW_ODM_PHYDM_INIT_C_

#ifdef HOST_ODM_PHYDM_INIT_TEST
#include "host_odm_phydm_types.h"
#else
#include <rtw_odm.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_ODM_PHYDM_INIT_TEST) || !defined(CONFIG_RUST_ODM_PHYDM_INIT)
u32 rtw_phydm_ability_ops(_adapter *adapter, HAL_PHYDM_OPS ops, u32 ability)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(adapter);
	struct dm_struct *podmpriv = &pHalData->odmpriv;
	u32 result = 0;

	switch (ops) {
	case HAL_PHYDM_DIS_ALL_FUNC:
		podmpriv->support_ability = DYNAMIC_FUNC_DISABLE;
		halrf_cmn_info_set(podmpriv, HALRF_CMNINFO_ABILITY, DYNAMIC_FUNC_DISABLE);
		break;
	case HAL_PHYDM_FUNC_SET:
		podmpriv->support_ability |= ability;
		break;
	case HAL_PHYDM_FUNC_CLR:
		podmpriv->support_ability &= ~(ability);
		break;
	case HAL_PHYDM_ABILITY_BK:
		podmpriv->bk_support_ability = podmpriv->support_ability;
		pHalData->bk_rf_ability = halrf_cmn_info_get(podmpriv, HALRF_CMNINFO_ABILITY);
		break;
	case HAL_PHYDM_ABILITY_RESTORE:
		podmpriv->support_ability = podmpriv->bk_support_ability;
		halrf_cmn_info_set(podmpriv, HALRF_CMNINFO_ABILITY, pHalData->bk_rf_ability);
		break;
	case HAL_PHYDM_ABILITY_SET:
		podmpriv->support_ability = ability;
		break;
	case HAL_PHYDM_ABILITY_GET:
		result = podmpriv->support_ability;
		break;
	}
	return result;
}

void rtw_odm_init_ic_type(_adapter *adapter)
{
	struct dm_struct *odm = adapter_to_phydm(adapter);
	u32 ic_type = chip_type_to_odm_ic_type(rtw_get_chip_type(adapter));

	rtw_warn_on(!ic_type);

	odm_cmn_info_init(odm, ODM_CMNINFO_IC_TYPE, ic_type);
}
#endif

#ifndef HOST_ODM_PHYDM_INIT_TEST
#if defined(CONFIG_RUST) && defined(CONFIG_RUST_ODM_PHYDM_INIT)
u32 rtw_phydm_ability_ops(_adapter *adapter, HAL_PHYDM_OPS ops, u32 ability);
void rtw_odm_init_ic_type(_adapter *adapter);
#endif
#endif
