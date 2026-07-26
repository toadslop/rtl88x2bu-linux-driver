// SPDX-License-Identifier: GPL-2.0
/*
 * HAL band-cap shim for host L2 chplan init_channel_set tests (W2-20).
 */
#include "host_chplan_types.h"

static u8 host_band_cap = BAND_CAP_2G | BAND_CAP_5G;

void host_chplan_set_band_cap(u8 cap)
{
	host_band_cap = cap;
}

bool hal_chk_band_cap(_adapter *adapter, u8 cap)
{
	(void)adapter;
	return (host_band_cap & cap) != 0;
}

u8 rtw_os_init_channel_set(_adapter *padapter, RT_CHANNEL_INFO *channel_set)
{
	(void)padapter;
	(void)channel_set;
	return 0;
}
