// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-112 — mirrors hal/hal_dm.c rtw_phydm_read_efuse. */

#include "host_hal_dm_phydm_read_efuse_trim_types.h"

void rtw_phydm_read_efuse(_adapter *adapter)
{
	struct dm_struct *phydm = &(GET_HAL_DATA(adapter)->odmpriv);

	phydm_get_thermal_trim_offset(phydm);
	phydm_get_power_trim_offset(phydm);
}