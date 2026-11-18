// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-117 STA RA info orchestration L2 harness. */

#include "host_hal_com_update_sta_ra_info_types.h"

void rtw_hal_update_sta_mimo_type(struct host_adapter_ra *adapter, struct host_sta_ra *psta)
{
	(void)adapter;
	(void)psta;
	host_hal_com_update_sta_ra_info_record_step(HOST_RA_STEP_MIMO);
	host_hal_com_update_sta_ra_info_trace.mimo_calls++;
}

void rtw_hal_update_sta_smps_cap(struct host_adapter_ra *adapter, struct host_sta_ra *psta)
{
	(void)adapter;
	(void)psta;
	host_hal_com_update_sta_ra_info_record_step(HOST_RA_STEP_SMPS);
	host_hal_com_update_sta_ra_info_trace.smps_calls++;
}

void rtw_hal_update_sta_rate_mask(struct host_adapter_ra *adapter, struct host_sta_ra *psta)
{
	(void)adapter;
	(void)psta;
	host_hal_com_update_sta_ra_info_record_step(HOST_RA_STEP_RATE_MASK);
	host_hal_com_update_sta_ra_info_trace.rate_mask_calls++;
}