// SPDX-License-Identifier: GPL-2.0
/* Host shims for Rust W4-102 RRSR control oracle. */

#include "host_hal_dm_phydm_rrsr_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}

u32 rtw_rust_hal_dm_registry_set_rrsr_value(_adapter *adapter)
{
	if (!adapter)
		return 0xFFFFFFFF;
	return adapter->registrypriv.set_rrsr_value;
}