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
