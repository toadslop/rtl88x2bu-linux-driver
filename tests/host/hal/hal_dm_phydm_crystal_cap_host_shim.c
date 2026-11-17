// SPDX-License-Identifier: GPL-2.0
/* Host shims for Rust W4-104 crystal cap oracle. */

#include "host_hal_dm_phydm_crystal_cap_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}