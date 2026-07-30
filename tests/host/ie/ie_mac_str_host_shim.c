// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 shim for W3-30 mac/str helpers: module globals and deterministic RNG.
 */
#include "host_ieee80211_types.h"

char *rtw_initmac = NULL;

static u32 host_random32 = 0xAABBCCDD;

u32 rtw_random32(void)
{
	return host_random32;
}

void host_mac_str_test_set_initmac(const char *mac)
{
	rtw_initmac = (char *)mac;
}

void host_mac_str_test_clear_initmac(void)
{
	rtw_initmac = NULL;
}

void host_mac_str_test_set_random32(u32 val)
{
	host_random32 = val;
}

#if defined(RUST_IEEE80211_REST_ORACLE) && !defined(HOST_RF_TEST)
/*
 * Satisfy rust/rtw_ieee80211_rest.rs chbw link for non-chbw IE rest
 * harnesses; only called if sync_chbw runs (chbw vectors use rf_rest).
 */
u8 rtw_get_offset_by_chbw(u8 ch, u8 bw, u8 *r_offset)
{
	(void)ch;
	(void)bw;
	if (r_offset)
		*r_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	return 0;
}
#endif
