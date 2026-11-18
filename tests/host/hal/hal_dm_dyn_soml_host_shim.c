// SPDX-License-Identifier: GPL-2.0
/* Host PHYDM / hwreg mocks for W4-111 L2. */

#include <string.h>

#include "host_hal_dm_dyn_soml_types.h"

struct host_hal_dm_dyn_soml_trace host_hal_dm_dyn_soml_trace;

void host_hal_dm_dyn_soml_reset(void)
{
	memset(&host_hal_dm_dyn_soml_trace, 0, sizeof(host_hal_dm_dyn_soml_trace));
}

void phydm_soml_bytes_acq(void *dm_void, u8 rate_id, u32 length)
{
	(void)dm_void;
	host_hal_dm_dyn_soml_trace.bytes_acq_calls++;
	host_hal_dm_dyn_soml_trace.bytes_acq_rate = rate_id;
	host_hal_dm_dyn_soml_trace.bytes_acq_length = length;
}

void phydm_adaptive_soml_para_set(void *dm_void, u8 train_num, u8 intvl,
				  u8 period, u8 delay)
{
	(void)dm_void;
	host_hal_dm_dyn_soml_trace.para_set_calls++;
	host_hal_dm_dyn_soml_trace.para_train = train_num;
	host_hal_dm_dyn_soml_trace.para_intvl = intvl;
	host_hal_dm_dyn_soml_trace.para_period = period;
	host_hal_dm_dyn_soml_trace.para_delay = delay;
}

void rtw_hal_set_hwreg(_adapter *adapter, enum host_hw_var variable, u8 *val)
{
	(void)adapter;
	(void)val;
	host_hal_dm_dyn_soml_trace.set_hwreg_calls++;
	host_hal_dm_dyn_soml_trace.set_hwreg_var = (u32)variable;
}