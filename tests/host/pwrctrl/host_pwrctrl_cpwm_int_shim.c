/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-188 LPS_Leave_check + cpwm_int_hdl (CONFIG_LPS_LCLK). */

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef u32 systime;

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))
#define PS_MODE_ACTIVE 0
#define PS_TOGGLE BIT(7)
#define PS_STATE_MASK 0x0F
#define PS_STATE(x) (PS_STATE_MASK & (x))
#define PS_STATE_S2 (1 << 1)
#define CMD_ALIVE BIT(2)
#define XMIT_ALIVE BIT(0)

struct reportpwrstate_parm {
	unsigned char mode;
	unsigned char state;
	unsigned short rsvd;
};

struct pwrctrl_priv {
	int lock;
	u8 pwr_mode;
	u8 cpwm;
	u8 cpwm_tog;
	u8 rpwm;
	u8 alives;
	u8 _pad[3];
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
	u8 bSurpriseRemoved;
	u8 bHwInitCompleted;
	u8 bDriverStopped;
	u8 _pad;
	u32 cmd_sem_up;
	u32 xmit_sem_up;
};

static systime g_time;

void host_pwrctrl_cpwm_int_reset(void)
{
	g_time = 0;
}

void host_pwrctrl_cpwm_int_set_time(systime t)
{
	g_time = t;
}

u8 host_pwrctrl_cpwm_int_get_cpwm(void *adapter)
{
	return ((struct adapter *)adapter)->pwrctrlpriv.cpwm;
}

u8 host_pwrctrl_cpwm_int_get_cpwm_tog(void *adapter)
{
	return ((struct adapter *)adapter)->pwrctrlpriv.cpwm_tog;
}

u32 host_pwrctrl_cpwm_int_cmd_sem_up(void *adapter)
{
	return ((struct adapter *)adapter)->cmd_sem_up;
}

u32 host_pwrctrl_cpwm_int_xmit_sem_up(void *adapter)
{
	return ((struct adapter *)adapter)->xmit_sem_up;
}

static systime rtw_get_current_time(void)
{
	return g_time;
}

static u32 rtw_get_passing_time_ms(systime start)
{
	return g_time - start;
}

static void rtw_yield_os(void) {}

static void rtw_msleep_os(int ms)
{
	g_time += (u32)ms;
}

static void _enter_pwrlock(int *lock)
{
	(void)lock;
}

static void _exit_pwrlock(int *lock)
{
	(void)lock;
}

#define RTW_CANNOT_RUN(a) \
	((a)->bSurpriseRemoved || (a)->bDriverStopped)

void LPS_Leave_check(void *padapter)
{
	struct adapter *a = padapter;
	struct pwrctrl_priv *pwrpriv = &a->pwrctrlpriv;
	systime start_time;
	u8 bReady;

	bReady = _FALSE;
	start_time = rtw_get_current_time();
	rtw_yield_os();

	while (1) {
		_enter_pwrlock(&pwrpriv->lock);

		if (a->bSurpriseRemoved || !a->bHwInitCompleted || a->bDriverStopped ||
		    pwrpriv->pwr_mode == PS_MODE_ACTIVE)
			bReady = _TRUE;

		_exit_pwrlock(&pwrpriv->lock);

		if (bReady == _TRUE)
			break;

		if (rtw_get_passing_time_ms(start_time) > 100)
			break;

		rtw_msleep_os(1);
	}
}

void cpwm_int_hdl(void *padapter, struct reportpwrstate_parm *preportpwrstate)
{
	struct adapter *a = padapter;
	struct pwrctrl_priv *pwrpriv;

	if (!a)
		return;

	if (RTW_CANNOT_RUN(a))
		return;

	pwrpriv = &a->pwrctrlpriv;

	_enter_pwrlock(&pwrpriv->lock);

	pwrpriv->cpwm = PS_STATE(preportpwrstate->state);
	pwrpriv->cpwm_tog = preportpwrstate->state & PS_TOGGLE;

	if (pwrpriv->cpwm >= PS_STATE_S2) {
		if (pwrpriv->alives & CMD_ALIVE)
			a->cmd_sem_up++;
		if (pwrpriv->alives & XMIT_ALIVE)
			a->xmit_sem_up++;
	}

	_exit_pwrlock(&pwrpriv->lock);
}
