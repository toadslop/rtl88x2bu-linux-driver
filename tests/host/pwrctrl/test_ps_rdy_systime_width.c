// SPDX-License-Identifier: GPL-2.0
/* Kernel-path PS_RDY_CHECK must compare full systime, not a u32 truncation.
 *
 * jiffies starts at INITIAL_JIFFIES = (u32)(-300*HZ), so it crosses 2^32
 * about five minutes after boot. lps_deny_time is stored as that clock and
 * is not cleared when it expires. Truncating both sides to u32 makes a
 * pre-boundary stamp look like it is still in the future.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>

extern unsigned char PS_RDY_CHECK(void *padapter);

static unsigned long g_now;
static unsigned long g_deny;

unsigned long rtw_rust_ps_rdy_get_current_time(void)
{
	return g_now;
}

unsigned long rtw_rust_ps_rdy_get_lps_deny_time(void *adapter)
{
	(void)adapter;
	return g_deny;
}

unsigned char rtw_rust_ps_rdy_time_after(unsigned long a, unsigned long b)
{
	/* include/linux/jiffies.h time_after() */
	return ((long)(b - a) < 0) ? 1 : 0;
}

unsigned char rtw_rust_ps_rdy_check_fwstate(void *adapter, unsigned int state)
{
	(void)adapter;
	(void)state;
	return 0;
}

unsigned char rtw_rust_ps_rdy_mlme_is_ap(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_mlme_is_mesh(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_mlme_is_monitor(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_get_b_in_suspend(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_get_wowlan_mode(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_get_wowlan_ap_mode(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_cfg80211_get_is_roch(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_is_scan_deny(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_tdls_linked(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_radar_detect(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned int rtw_rust_ps_rdy_get_dot11_auth(void *adapter)
{
	(void)adapter;
	return 0;
}

unsigned char rtw_rust_ps_rdy_get_binstall_grpkey(void *adapter)
{
	(void)adapter;
	return 1;
}

unsigned char rtw_rust_ps_rdy_cfg80211_pwr_mgmt(void *adapter)
{
	(void)adapter;
	return 1;
}

void rtw_rust_ps_rdy_log_group_handshake_in_progress(void)
{
}

static int expect_ready(const char *name, unsigned long deny, unsigned long now, unsigned char want)
{
	void *adapter = (void *)(uintptr_t)1;
	unsigned char got;

	g_deny = deny;
	g_now = now;
	got = PS_RDY_CHECK(adapter);
	if (got != want) {
		fprintf(stderr, "%s: deny=%lu now=%lu got %u want %u\n",
			name, deny, now, got, want);
		return 1;
	}
	return 0;
}

#if ULONG_MAX <= 0xffffffffUL
int main(void)
{
	/* systime is 32-bit here; the truncation gap does not exist. */
	return 0;
}
#else
int main(void)
{
	const unsigned long initial = (unsigned long)(unsigned int)(-300 * 1000);
	const unsigned long boundary = 1UL << 32;
	const unsigned long deny_at_probe = initial + 1000;
	const unsigned long after_boundary = boundary + 10000;
	int rc = 0;

	/* Stale probe-time stamp, checked after jiffies crosses 2^32. */
	rc |= expect_ready("pre-boundary deny expired", deny_at_probe, after_boundary, 1);
	/* Deny still ahead on the full clock. */
	rc |= expect_ready("post-boundary deny future", after_boundary + 1000, after_boundary, 0);
	/* Deny already passed inside the same epoch. */
	rc |= expect_ready("post-boundary deny past", after_boundary - 1000, after_boundary, 1);
	return rc;
}
#endif
