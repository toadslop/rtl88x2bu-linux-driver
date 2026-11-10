// SPDX-License-Identifier: GPL-2.0
/* Kernel-path FX-01: rpwm polling and cpwm LPS leave must keep full systime
 * width. Jiffies past 2^32 with start captured as u32 makes passing time
 * explode on CONFIG_HZ=300 (see fix-01-pwrctrl-systime-width.md).
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define PS_TOGGLE (1U << 7)
#define PS_STATE_S4 ((1U << 3) | (1U << 2))

/* --- shared fake HZ=300 clock ------------------------------------------- */

static unsigned long g_now;
static unsigned int g_leave_timeout_err;

static unsigned long jiffies_per_ms(void)
{
	return 300U;
}

static unsigned long jiffies_add_ms(unsigned long j, unsigned int ms)
{
	return j + ms * jiffies_per_ms();
}

static unsigned int jiffies_to_msecs_h300(unsigned long delta)
{
	/* include/linux/jiffies.h: jiffies_to_msecs — 1000/HZ not integral */
	return (unsigned int)((delta * 10ULL + 2ULL) / 3ULL);
}

/* --- rpwm / cpwm_polling stubs ------------------------------------------ */

typedef uintptr_t PADAPTER;
typedef unsigned char u8;
typedef uint32_t u32;

struct pwrctrl_priv_rpwm {
	u8 cpwm;
	u8 cpwm_tog;
	u8 tog;
	u8 rpwm_retry;
};

struct adapter_rpwm {
	struct pwrctrl_priv_rpwm pwrctrlpriv;
};

static u8 g_cpwm_val;
static unsigned int g_cpwm_reads;
static unsigned int g_reads_before_toggle;

extern u8 rtw_cpwm_polling(PADAPTER adapter, u8 rpwm, u8 cpwm_orig);

unsigned long rtw_rust_rpwm_get_current_time(void)
{
	return g_now;
}

unsigned int rtw_rust_rpwm_get_passing_time_ms(unsigned long start)
{
	return jiffies_to_msecs_h300(g_now - start);
}

void rtw_rust_rpwm_msleep_os(int ms)
{
	g_now = jiffies_add_ms(g_now, (unsigned int)ms);
}

void rtw_rust_rpwm_hal_get_cpwm(PADAPTER padapter, u8 *out)
{
	(void)padapter;
	g_cpwm_reads++;
	if (g_cpwm_reads > g_reads_before_toggle)
		g_cpwm_val ^= PS_TOGGLE;
	*out = g_cpwm_val;
}

void rtw_rust_rpwm_hal_set_rpwm(PADAPTER padapter, u8 rpwm)
{
	(void)padapter;
	(void)rpwm;
}

u8 rtw_rust_rpwm_cannot_run(PADAPTER padapter)
{
	(void)padapter;
	return 0;
}

u8 rtw_rust_rpwm_get_u8(PADAPTER padapter, u32 field)
{
	struct adapter_rpwm *a = (struct adapter_rpwm *)padapter;

	switch (field) {
	case 0:
		return a->pwrctrlpriv.cpwm;
	case 1:
		return a->pwrctrlpriv.cpwm_tog;
	case 2:
		return a->pwrctrlpriv.tog;
	default:
		return 0;
	}
}

void rtw_rust_rpwm_set_u8(PADAPTER padapter, u32 field, u8 v)
{
	struct adapter_rpwm *a = (struct adapter_rpwm *)padapter;

	switch (field) {
	case 0:
		a->pwrctrlpriv.cpwm = v;
		break;
	case 1:
		a->pwrctrlpriv.cpwm_tog = v;
		break;
	case 2:
		a->pwrctrlpriv.tog = v;
		break;
	default:
		break;
	}
}

void rtw_rust_rpwm_reset_rpwm_retry(PADAPTER padapter)
{
	struct adapter_rpwm *a = (struct adapter_rpwm *)padapter;

	a->pwrctrlpriv.rpwm_retry = 0;
}

u8 rtw_rust_rpwm_bump_rpwm_retry(PADAPTER padapter)
{
	struct adapter_rpwm *a = (struct adapter_rpwm *)padapter;

	return a->pwrctrlpriv.rpwm_retry++;
}

void rtw_rust_rpwm_poll_complete(PADAPTER padapter, u8 rst, u8 cpwm_orig,
				 u8 cpwm_now)
{
	(void)padapter;
	(void)rst;
	(void)cpwm_orig;
	(void)cpwm_now;
}

/* --- cpwm LPS_Leave_check stubs ----------------------------------------- */

struct adapter_cpwm {
	u8 pwr_mode;
	u8 leave_ready;
};

extern void LPS_Leave_check(PADAPTER padapter);

void rtw_rust_cpwm_enter_pwrlock(PADAPTER padapter) { (void)padapter; }
void rtw_rust_cpwm_exit_pwrlock(PADAPTER padapter) { (void)padapter; }
void rtw_rust_cpwm_yield_os(void) {}

unsigned long rtw_rust_cpwm_get_current_time(void)
{
	return g_now;
}

unsigned int rtw_rust_cpwm_get_passing_time_ms(unsigned long start)
{
	return jiffies_to_msecs_h300(g_now - start);
}

void rtw_rust_cpwm_msleep_os(int ms)
{
	g_now = jiffies_add_ms(g_now, (unsigned int)ms);
}

void rtw_rust_cpwm_leave_timeout_err(void)
{
	g_leave_timeout_err++;
}

u8 rtw_rust_cpwm_leave_ready(PADAPTER padapter)
{
	return ((struct adapter_cpwm *)padapter)->leave_ready;
}

u8 rtw_rust_cpwm_cannot_run(PADAPTER padapter) { (void)padapter; return 0; }
void rtw_rust_cpwm_set_cpwm_fields(PADAPTER padapter, u8 state)
{
	(void)padapter;
	(void)state;
}
u8 rtw_rust_cpwm_get_alives(PADAPTER padapter)
{
	(void)padapter;
	return 0;
}
void rtw_rust_cpwm_up_cmd_sema(PADAPTER padapter) { (void)padapter; }
void rtw_rust_cpwm_up_xmit_sema(PADAPTER padapter) { (void)padapter; }
u8 rtw_rust_cpwm_rpwm_timer_redundant(PADAPTER padapter)
{
	(void)padapter;
	return 0;
}

#if ULONG_MAX <= 0xffffffffUL
int main(void)
{
	return 0;
}
#else
static int test_cpwm_polling_post_boundary(void)
{
	struct adapter_rpwm a;
	u8 ret;

	memset(&a, 0, sizeof(a));
	g_now = (1UL << 32) + 0x10UL;
	g_cpwm_val = 0x00;
	g_cpwm_reads = 0;
	g_reads_before_toggle = 1;
	g_leave_timeout_err = 0;

	ret = rtw_cpwm_polling((PADAPTER)&a, 0, g_cpwm_val);
	if (ret != _SUCCESS) {
		fprintf(stderr, "cpwm_polling: post-boundary start failed (%u)\n", ret);
		return 1;
	}
	return 0;
}

static int test_lps_leave_check_post_boundary(void)
{
	struct adapter_cpwm a;

	memset(&a, 0, sizeof(a));
	a.pwr_mode = 0;
	a.leave_ready = _TRUE;
	g_now = (1UL << 32) + 0x10UL;
	g_leave_timeout_err = 0;

	LPS_Leave_check((PADAPTER)&a);
	if (g_leave_timeout_err != 0) {
		fprintf(stderr, "LPS_Leave_check: spurious timeout after boundary\n");
		return 1;
	}
	return 0;
}

int main(void)
{
	int rc = 0;

	rc |= test_cpwm_polling_post_boundary();
	rc |= test_lps_leave_check_post_boundary();
	return rc;
}
#endif
