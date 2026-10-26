/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-173 rtw_ps_processor (IPS path, no HW RFOFF). */

#include "host_pwrctrl_ps_processor.h"

#include <string.h>

static struct host_pwrctrl_ps_processor_trace g_trace;
static u8 g_unassoc_idle = _TRUE;

void _enter_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void _exit_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

u32 rtw_ps_deny_get(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv;

	if (!padapter)
		return 0;
	pwrpriv = adapter_to_pwrctl(padapter);
	_enter_pwrlock(&pwrpriv->lock);
	u32 deny = pwrpriv->ps_deny;
	_exit_pwrlock(&pwrpriv->lock);
	return deny;
}

static void ips_enter(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ips_enter++;
}

static u8 rtw_pwr_unassociated_idle(_adapter *adapter)
{
	(void)adapter;
	return g_unassoc_idle;
}

#ifndef CONFIG_IPS_CHECK_IN_WD
static void rtw_set_pwr_state_check_timer(struct pwrctrl_priv *pwrctl)
{
	(void)pwrctl;
	g_trace.pwr_timer_set++;
}
#else
static void rtw_set_pwr_state_check_timer(struct pwrctrl_priv *pwrctl)
{
	(void)pwrctl;
}
#endif

void host_pwrctrl_ps_proc_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
	g_unassoc_idle = _TRUE;
}

struct host_pwrctrl_ps_processor_trace *host_pwrctrl_ps_proc_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_ps_proc_set_unassoc_idle(u8 idle)
{
	g_unassoc_idle = idle;
}

void rtw_ps_processor(_adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	u32 ps_deny;

	_enter_pwrlock(&adapter_to_pwrctl(padapter)->lock);
	ps_deny = rtw_ps_deny_get(padapter);
	_exit_pwrlock(&adapter_to_pwrctl(padapter)->lock);
	if (ps_deny != 0)
		goto exit;

	if (pwrpriv->bInSuspend == _TRUE)
		return;

	pwrpriv->ps_processing = _TRUE;

	if (pwrpriv->ips_mode_req == IPS_NONE)
		goto exit;

	if (rtw_pwr_unassociated_idle(padapter) == _FALSE)
		goto exit;

	if ((pwrpriv->rf_pwrstate == rf_on) &&
	    ((pwrpriv->pwr_state_check_cnts % 4) == 0)) {
		pwrpriv->change_rfpwrstate = rf_off;
		ips_enter(padapter);
	}

exit:
#ifndef CONFIG_IPS_CHECK_IN_WD
	rtw_set_pwr_state_check_timer(pwrpriv);
#endif
	pwrpriv->ps_processing = _FALSE;
}
