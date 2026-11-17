// SPDX-License-Identifier: GPL-2.0
/* Host shims for Rust W4-105 IQK debug trigger oracle. */

#include "host_hal_dm_phydm_iqk_trigger_dbg_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}