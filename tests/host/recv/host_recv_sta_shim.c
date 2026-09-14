// SPDX-License-Identifier: GPL-2.0
#include "host_recv_sta_types.h"

static struct sta_info host_sta;
static u8 host_sta_mac[ETH_ALEN];
static u8 host_sta_valid;
static u32 host_deauth_count;
static systime host_now = 1000;
static s32 host_passing_ms = 20000;

void host_recv_sta_reset(void)
{
	memset(&host_sta, 0, sizeof(host_sta));
	memset(host_sta_mac, 0, sizeof(host_sta_mac));
	host_sta_valid = 0;
	host_deauth_count = 0;
}

void host_recv_sta_register_sta(const u8 *mac, struct sta_info *sta)
{
	memcpy(host_sta_mac, mac, ETH_ALEN);
	host_sta = *sta;
	host_sta_valid = 1;
}

u32 host_recv_sta_deauth_count(void)
{
	return host_deauth_count;
}

void host_recv_sta_set_time(systime now, s32 passing_ms)
{
	host_now = now;
	host_passing_ms = passing_ms;
}

systime rtw_get_current_time(void)
{
	return host_now;
}

s32 rtw_get_passing_time_ms(systime start)
{
	return (s32)(host_now - start);
}

void issue_deauth(_adapter *adapter, u8 *mac, u16 reason)
{
	(void)adapter;
	(void)mac;
	(void)reason;
	host_deauth_count++;
}

struct sta_info *rtw_get_stainfo(struct sta_priv *stapriv, u8 *hwaddr)
{
	(void)stapriv;
	if (!host_sta_valid)
		return NULL;
	if (memcmp(host_sta_mac, hwaddr, ETH_ALEN) != 0)
		return NULL;
	return &host_sta;
}
