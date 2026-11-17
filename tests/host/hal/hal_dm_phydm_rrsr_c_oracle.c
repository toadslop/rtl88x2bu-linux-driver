// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-102 RRSR helpers (hal/hal_dm.c). */

#include "host_hal_dm_phydm_rrsr_types.h"

void rtw_phydm_set_rrsr(_adapter *adapter, u32 rrsr_value, boolean write_rrsr)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u32 temp_rrsr = 0xFFFFFFFF;

	if (adapter->registrypriv.set_rrsr_value != 0xFFFFFFFF)
		temp_rrsr = adapter->registrypriv.set_rrsr_value;
	else
		temp_rrsr = rrsr_value;

	odm_cmn_info_update(phydm, ODM_CMNINFO_RRSR_VAL, temp_rrsr);
	if (write_rrsr)
		phydm_rrsr_set_register(phydm, temp_rrsr);
}

void rtw_phydm_dyn_rrsr_en(_adapter *adapter, boolean en_rrsr)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);

	phydm_rrsr_en(phydm, en_rrsr);
}