// SPDX-License-Identifier: GPL-2.0
/* Kernel FX-01: systime width for rpwm cpwm polling + cpwm LPS leave (HZ=300). */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define _SUCCESS 1
#define _TRUE 1
#define PS_TOGGLE (1U << 7)
#define PS_STATE_S4 ((1U << 3) | (1U << 2))

typedef uintptr_t PADAPTER;
typedef unsigned char u8;
typedef uint32_t u32;

static unsigned long g_now;
static unsigned int g_leave_timeout_err;
static u8 g_cpwm_val;
static unsigned int g_cpwm_reads, g_reads_before_toggle;

static unsigned long j_add_ms(unsigned long j, unsigned int ms)
{
	return j + ms * 300U;
}

static unsigned int j_to_ms(unsigned long delta)
{
	return (unsigned int)((delta * 10ULL + 2ULL) / 3ULL);
}

#define NOW() g_now
#define PASSING_MS(start) j_to_ms(g_now - (start))

#define TIME_STUBS(sym) \
	unsigned long sym##_get_current_time(void) { return NOW(); } \
	unsigned int sym##_get_passing_time_ms(unsigned long s) { return PASSING_MS(s); } \
	void sym##_msleep_os(int ms) { g_now = j_add_ms(g_now, (unsigned int)ms); }

TIME_STUBS(rtw_rust_rpwm)
TIME_STUBS(rtw_rust_cpwm)

struct adapter_rpwm {
	u8 cpwm, cpwm_tog, tog, rpwm_retry;
};
struct adapter_cpwm {
	u8 pwr_mode, leave_ready;
};

extern u8 rtw_cpwm_polling(PADAPTER, u8, u8);
extern void LPS_Leave_check(PADAPTER);

void rtw_rust_rpwm_hal_get_cpwm(PADAPTER a, u8 *out)
{
	(void)a;
	if (++g_cpwm_reads > g_reads_before_toggle)
		g_cpwm_val ^= PS_TOGGLE;
	*out = g_cpwm_val;
}
void rtw_rust_rpwm_hal_set_rpwm(PADAPTER a, u8 r) { (void)a; (void)r; }
u8 rtw_rust_rpwm_cannot_run(PADAPTER a) { (void)a; return 0; }
u8 rtw_rust_rpwm_get_u8(PADAPTER a, u32 f)
{
	struct adapter_rpwm *x = (struct adapter_rpwm *)a;
	return f == 2 ? x->tog : (f ? x->cpwm_tog : x->cpwm);
}
void rtw_rust_rpwm_set_u8(PADAPTER a, u32 f, u8 v)
{
	struct adapter_rpwm *x = (struct adapter_rpwm *)a;
	if (!f)
		x->cpwm = v;
	else if (f == 1)
		x->cpwm_tog = v;
	else
		x->tog = v;
}
void rtw_rust_rpwm_reset_rpwm_retry(PADAPTER a) { ((struct adapter_rpwm *)a)->rpwm_retry = 0; }
u8 rtw_rust_rpwm_bump_rpwm_retry(PADAPTER a) { return ((struct adapter_rpwm *)a)->rpwm_retry++; }
void rtw_rust_rpwm_poll_complete(PADAPTER a, u8 r, u8 o, u8 n)
{
	(void)a; (void)r; (void)o; (void)n;
}

void rtw_rust_cpwm_enter_pwrlock(PADAPTER a) { (void)a; }
void rtw_rust_cpwm_exit_pwrlock(PADAPTER a) { (void)a; }
void rtw_rust_cpwm_yield_os(void) {}
void rtw_rust_cpwm_leave_timeout_err(void) { g_leave_timeout_err++; }
u8 rtw_rust_cpwm_leave_ready(PADAPTER a)
{
	return ((struct adapter_cpwm *)a)->leave_ready;
}
u8 rtw_rust_cpwm_cannot_run(PADAPTER a) { (void)a; return 0; }
void rtw_rust_cpwm_set_cpwm_fields(PADAPTER a, u8 s) { (void)a; (void)s; }
u8 rtw_rust_cpwm_get_alives(PADAPTER a) { (void)a; return 0; }
void rtw_rust_cpwm_up_cmd_sema(PADAPTER a) { (void)a; }
void rtw_rust_cpwm_up_xmit_sema(PADAPTER a) { (void)a; }
u8 rtw_rust_cpwm_rpwm_timer_redundant(PADAPTER a) { (void)a; return 0; }

#if ULONG_MAX <= 0xffffffffUL
int main(void) { return 0; }
#else
int main(void)
{
	struct adapter_rpwm ar;
	struct adapter_cpwm ac;
	int rc = 0;

	memset(&ar, 0, sizeof(ar));
	g_now = (1UL << 32) + 0x10UL;
	g_cpwm_val = g_cpwm_reads = 0;
	g_reads_before_toggle = 1;
	if (rtw_cpwm_polling((PADAPTER)&ar, 0, 0) != _SUCCESS) {
		fprintf(stderr, "cpwm_polling: post-boundary fail\n");
		rc |= 1;
	}

	memset(&ac, 0, sizeof(ac));
	ac.leave_ready = _TRUE;
	g_now = (1UL << 32) + 0x10UL;
	g_leave_timeout_err = 0;
	LPS_Leave_check((PADAPTER)&ac);
	if (g_leave_timeout_err) {
		fprintf(stderr, "LPS_Leave_check: spurious timeout\n");
		rc |= 1;
	}
	return rc;
}
#endif
