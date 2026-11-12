// SPDX-License-Identifier: GPL-2.0
/* Kernel-path wakeup ips_deny must use full systime (FX-01 / T24). */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uintptr_t PADAPTER;
typedef unsigned long systime;
typedef unsigned int u32;
typedef unsigned char u8;

static systime g_now;
static systime g_deny;

extern void rtw_set_ips_deny(PADAPTER padapter, u32 ms);

systime rtw_rust_wakeup_get_current_time(void)
{
	return g_now;
}

systime rtw_rust_wakeup_ms_to_systime(u32 ms)
{
	return (systime)ms;
}

u8 rtw_rust_wakeup_time_after(systime a, systime b)
{
	return ((long)(b - a) < 0) ? 1 : 0;
}

u32 rtw_rust_wakeup_get_passing_time_ms(systime start)
{
	(void)start;
	return 0;
}

void rtw_rust_wakeup_msleep_os(u32 ms) { (void)ms; }

struct pwrctrl_priv {
	systime ips_deny_time;
};

static struct pwrctrl_priv g_pwr;

void *rtw_rust_wakeup_pwrpriv(PADAPTER adapter)
{
	(void)adapter;
	return &g_pwr;
}

void rtw_rust_wakeup_set_ips_deny_time(void *pwr, systime t)
{
	((struct pwrctrl_priv *)pwr)->ips_deny_time = t;
}

systime rtw_rust_wakeup_get_ips_deny_time(void *pwr)
{
	return ((struct pwrctrl_priv *)pwr)->ips_deny_time;
}

void rtw_rust_wakeup_leave_all_ps(PADAPTER a) { (void)a; }
PADAPTER rtw_rust_wakeup_get_primary(PADAPTER a) { return a; }
u8 rtw_rust_wakeup_get_ps_processing(void *p) { (void)p; return 0; }
u8 rtw_rust_wakeup_get_b_in_suspend(void *p) { (void)p; return 0; }
u8 rtw_rust_wakeup_is_do_late_resume(void *p) { (void)p; return 0; }
u8 rtw_rust_wakeup_check_fwstate(PADAPTER a, u32 s) { (void)a; (void)s; return 0; }
u8 rtw_rust_wakeup_get_rf_pwrstate(void *p) { (void)p; return 0; }
int rtw_rust_wakeup_ips_leave(PADAPTER a) { (void)a; return 1; }
u8 rtw_rust_wakeup_is_drv_stopped(PADAPTER a) { (void)a; return 0; }
u8 rtw_rust_wakeup_get_bup(PADAPTER a) { (void)a; return 1; }
u8 rtw_rust_wakeup_is_hw_init_completed(PADAPTER a) { (void)a; return 1; }
u8 rtw_rust_wakeup_sreset_inprogress(PADAPTER a) { (void)a; return 0; }

static int expect_bump(const char *name, systime old_deny, systime now,
		       u32 defer_ms, systime want)
{
	g_pwr.ips_deny_time = old_deny;
	g_now = now;
	g_deny = old_deny;
	rtw_set_ips_deny((PADAPTER)0x1, defer_ms);
	if (g_pwr.ips_deny_time != want) {
		fprintf(stderr, "%s: deny=%lu (want %lu)\n", name, g_pwr.ips_deny_time,
			want);
		return 1;
	}
	return 0;
}

#if ULONG_MAX <= 0xffffffffUL
int main(void) { return 0; }
#else
int main(void)
{
	const systime boundary = 1UL << 32;
	const systime stale = boundary - 1000UL;
	const systime now = boundary + 10000UL;
	int rc = 0;

	/* Stale deny from before the boundary must not block a post-boundary bump. */
	rc |= expect_bump("stale deny bump", stale, now, 5000, now + 5000);
	/* Direct set always uses full-width now + ms. */
	memset(&g_pwr, 0, sizeof(g_pwr));
	g_now = now;
	rtw_set_ips_deny((PADAPTER)0x1, 32);
	if (g_pwr.ips_deny_time != now + 32) {
		fprintf(stderr, "set deny: got %lu want %lu\n", g_pwr.ips_deny_time,
			now + 32);
		rc |= 1;
	}
	return rc;
}
#endif
