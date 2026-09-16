// SPDX-License-Identifier: GPL-2.0
/* Minimal ieee80211 helpers for update_bmc_sta host oracle — sync with core/rtw_ieee80211_rest.c */
#include "host_ap_bmc_update_types.h"

#define _TRUE 1
#define _FALSE 0

static u8 rtw_is_cckrates_included(u8 *rate)
{
	u8 i = 0;

	while (rate[i] != 0) {
		if (((rate[i] & 0x7f) == 2) || ((rate[i] & 0x7f) == 4) ||
		    ((rate[i] & 0x7f) == 11) || ((rate[i] & 0x7f) == 22))
			return _TRUE;
		i++;
	}
	return _FALSE;
}

static u8 rtw_is_cckratesonly_included(u8 *rate)
{
	u8 i = 0;

	while (rate[i] != 0) {
		if (((rate[i] & 0x7f) != 2) && ((rate[i] & 0x7f) != 4) &&
		    ((rate[i] & 0x7f) != 11) && ((rate[i] & 0x7f) != 22))
			return _FALSE;
		i++;
	}
	return _TRUE;
}

u32 rtw_get_rateset_len(u8 *rateset)
{
	u32 i = 0;

	while (rateset[i] != 0 && i < HOST_BMC_MAX_RATES)
		i++;
	return i;
}

int rtw_check_network_type(u8 *rate, int ratelen, int channel)
{
	(void)ratelen;

	if (channel > 14) {
		if (rtw_is_cckrates_included(rate) == _TRUE)
			return WIRELESS_INVALID;
		return WIRELESS_11A;
	}
	if (rtw_is_cckratesonly_included(rate) == _TRUE)
		return WIRELESS_11B;
	if (rtw_is_cckrates_included(rate) == _TRUE)
		return WIRELESS_11BG;
	return WIRELESS_11G;
}
