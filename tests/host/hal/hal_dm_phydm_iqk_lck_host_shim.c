// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-98 IQK/LCK trigger L2 harness. */

#include "host_hal_dm_phydm_iqk_lck_types.h"

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? adapter_to_phydm(adapter) : ((struct dm_struct *)0);
}