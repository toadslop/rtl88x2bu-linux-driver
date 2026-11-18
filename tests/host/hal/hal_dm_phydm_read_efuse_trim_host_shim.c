// SPDX-License-Identifier: GPL-2.0
/* Host shims for Rust W4-112 efuse trim read hooks L2. */

#include <stddef.h>
#include <string.h>

#include "host_hal_dm_phydm_read_efuse_trim_types.h"

struct host_hal_dm_phydm_read_efuse_trim_trace host_hal_dm_phydm_read_efuse_trim_trace;

void host_hal_dm_phydm_read_efuse_trim_reset(void)
{
	memset(&host_hal_dm_phydm_read_efuse_trim_trace, 0,
	       sizeof(host_hal_dm_phydm_read_efuse_trim_trace));
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter ? &GET_HAL_DATA(adapter)->odmpriv : NULL;
}

void phydm_get_thermal_trim_offset(void *dm_void)
{
	(void)dm_void;
	host_hal_dm_phydm_read_efuse_trim_trace.thermal_trim_calls++;
}

void phydm_get_power_trim_offset(void *dm_void)
{
	(void)dm_void;
	host_hal_dm_phydm_read_efuse_trim_trace.power_trim_calls++;
}