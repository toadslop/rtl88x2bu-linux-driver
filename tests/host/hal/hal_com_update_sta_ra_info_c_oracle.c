// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-117 — mirrors hal/hal_com_rest.c rtw_hal_update_sta_ra_info. */

#include "host_hal_com_update_sta_ra_info_types.h"

void rtw_hal_update_sta_ra_info(struct host_adapter_ra *padapter, struct host_sta_ra *psta)
{
	rtw_hal_update_sta_mimo_type(padapter, psta);
	rtw_hal_update_sta_smps_cap(padapter, psta);
	rtw_hal_update_sta_rate_mask(padapter, psta);
}