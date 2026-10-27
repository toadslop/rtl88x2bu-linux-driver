/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-175 rtw_cpwm_polling (CONFIG_LPS_LCLK). */

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef u32 systime;

#define _SUCCESS 1
#define _FAIL 0
#define BIT(x) (1U << (x))
#define PS_TOGGLE BIT(7)
#define PS_ALL_ON BIT(2)
#define PS_ST_ACTIVE BIT(3)
#define PS_STATE_S4 ((PS_ST_ACTIVE) | (PS_ALL_ON))

struct pwrctrl_priv {
	u8 cpwm;
	u8 cpwm_tog;
	u8 tog;
	u8 rpwm_retry;
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
	/* Host stand-in for RTW_CANNOT_RUN (dvobj surprise-removed / drv-stopped). */
	u8 bSurpriseRemoved;
	u8 bDriverStopped;
};

#define HW_VAR_SET_RPWM 109
#define HW_VAR_CPWM 110

static systime g_time;
static u8 g_cpwm_orig;
static u32 g_poll_reads_before_toggle;
static u32 g_cpwm_read_count;

void host_pwrctrl_rpwm_reset_hal(void)
{
	g_time = 0;
	g_cpwm_orig = 0;
	g_poll_reads_before_toggle = 0;
	g_cpwm_read_count = 0;
}

void host_pwrctrl_rpwm_set_time(systime t)
{
	g_time = t;
}

void host_pwrctrl_rpwm_set_cpwm_behavior(u8 cpwm_orig, u32 poll_reads_before_toggle)
{
	g_cpwm_orig = cpwm_orig;
	g_poll_reads_before_toggle = poll_reads_before_toggle;
	g_cpwm_read_count = 0;
}

u8 host_pwrctrl_rpwm_get_cpwm(void *adapter)
{
	return ((struct adapter *)adapter)->pwrctrlpriv.cpwm;
}

u8 host_pwrctrl_rpwm_get_cpwm_tog(void *adapter)
{
	return ((struct adapter *)adapter)->pwrctrlpriv.cpwm_tog;
}

static systime rtw_get_current_time(void)
{
	return g_time;
}

static u32 rtw_get_passing_time_ms(systime start)
{
	return g_time - start;
}

static void rtw_msleep_os(int ms)
{
	g_time += (u32)ms;
}

static void rtw_hal_get_hwreg(struct adapter *a, int variable, u8 *val)
{
	u8 cpwm_now;

	(void)a;
	if (variable != HW_VAR_CPWM)
		return;
	g_cpwm_read_count++;
	cpwm_now = g_cpwm_orig;
	if (g_cpwm_read_count > g_poll_reads_before_toggle)
		cpwm_now ^= PS_TOGGLE;
	*val = cpwm_now;
}

static void rtw_hal_set_hwreg(struct adapter *a, int v, u8 *val)
{
	(void)a;
	(void)v;
	(void)val;
}

#define LPS_CPWM_TIMEOUT_MS 10
#define LPS_RPWM_RETRY_CNT 3

u8 rtw_cpwm_polling(void *adapter, u8 rpwm, u8 cpwm_orig)
{
	struct adapter *a = adapter;
	u8 rst = _FAIL;
	u8 cpwm_now = 0;
	systime start_time;
	struct pwrctrl_priv *pwrpriv = &a->pwrctrlpriv;

	pwrpriv->rpwm_retry = 0;

	do {
		start_time = rtw_get_current_time();
		do {
			rtw_msleep_os(1);
			rtw_hal_get_hwreg(a, HW_VAR_CPWM, &cpwm_now);
			if ((cpwm_orig ^ cpwm_now) & 0x80) {
				pwrpriv->cpwm = PS_STATE_S4;
				pwrpriv->cpwm_tog = cpwm_now & PS_TOGGLE;
				rst = _SUCCESS;
				break;
			}
		} while (rtw_get_passing_time_ms(start_time) < LPS_CPWM_TIMEOUT_MS &&
			 !(a->bSurpriseRemoved || a->bDriverStopped));

		if (rst == _SUCCESS)
			break;
		cpwm_orig = cpwm_now;
		rpwm &= ~PS_TOGGLE;
		rpwm |= pwrpriv->tog;
		rtw_hal_set_hwreg(a, HW_VAR_SET_RPWM, &rpwm);
		pwrpriv->tog += 0x80;
	} while (pwrpriv->rpwm_retry++ < LPS_RPWM_RETRY_CNT &&
		 !(a->bSurpriseRemoved || a->bDriverStopped));

	return rst;
}
