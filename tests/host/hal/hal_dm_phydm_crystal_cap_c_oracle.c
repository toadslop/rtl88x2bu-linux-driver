// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-104 — mirrors hal/hal_dm.c rtw_phydm_set_crystal_cap. */

#include "host_hal_dm_phydm_crystal_cap_types.h"

boolean rtw_phydm_set_crystal_cap(_adapter *adapter, u8 crystal_cap)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);

	return phydm_set_crystal_cap_reg(phydm, crystal_cap);
}