// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for W3-195 pwr wakeup + ips_deny — mirrors core/rtw_pwrctrl.c. */

#include "host_pwrctrl_wakeup.h"

#define RTW_INFO(fmt, ...) ((void)0)

#define rf_on 0
#define rf_off 2

static struct host_pwrctrl_wakeup_trace g_trace;
static systime g_time;
static u32 g_passing_ms;

void host_pwrctrl_wakeup_reset_trace(void)
{
	g_trace.leave_all_ps = 0;
	g_trace.ips_leave_calls = 0;
	g_trace.ips_leave_result = _SUCCESS;
}

struct host_pwrctrl_wakeup_trace *host_pwrctrl_wakeup_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_wakeup_set_ips_leave_result(s32 result)
{
	g_trace.ips_leave_result = result;
}

void host_pwrctrl_wakeup_set_passing_ms(u32 ms)
{
	g_passing_ms = ms;
}

void host_pwrctrl_wakeup_set_time(u32 t)
{
	g_time = t;
}

static systime rtw_get_current_time(void)
{
	return g_time;
}

static systime rtw_ms_to_systime(u32 ms)
{
	return ms;
}

static int rtw_time_after(systime a, systime b)
{
	return (s32)(a - b) > 0;
}

static u32 rtw_get_passing_time_ms(systime start)
{
	(void)start;
	return g_passing_ms;
}

static void rtw_msleep_os(unsigned int ms)
{
	(void)ms;
	g_passing_ms += 10;
}

static void LeaveAllPowerSaveMode(PADAPTER_WAKEUP padapter)
{
	(void)padapter;
	g_trace.leave_all_ps++;
}

static u8 rtw_is_do_late_resume(struct pwrctrl_priv_wakeup *pwrpriv)
{
	return pwrpriv->do_late_resume ? _TRUE : _FALSE;
}

static int check_fwstate(struct mlme_priv_wakeup *m, u32 state)
{
	return (m->fw_state & state) ? _TRUE : _FALSE;
}

static u8 rtw_is_drv_stopped(PADAPTER_WAKEUP padapter)
{
	(void)padapter;
	return _FALSE;
}

static u8 rtw_is_hw_init_completed(PADAPTER_WAKEUP padapter)
{
	(void)padapter;
	return _TRUE;
}

static int ips_leave(PADAPTER_WAKEUP padapter)
{
	(void)padapter;
	g_trace.ips_leave_calls++;
	return g_trace.ips_leave_result;
}

void rtw_set_ips_deny(PADAPTER_WAKEUP padapter, u32 ms)
{
	struct pwrctrl_priv_wakeup *pwrpriv = adapter_to_pwrctl_wakeup(padapter);

	pwrpriv->ips_deny_time = rtw_get_current_time() + rtw_ms_to_systime(ms);
}

int _rtw_pwr_wakeup(PADAPTER_WAKEUP padapter, u32 ips_deffer_ms, const char *caller)
{
	struct pwrctrl_priv_wakeup *pwrpriv = adapter_to_pwrctl_wakeup(padapter);
	struct mlme_priv_wakeup *pmlmepriv;
	systime start = rtw_get_current_time();
	int ret = _SUCCESS;

	(void)caller;
	LeaveAllPowerSaveMode(padapter);
	padapter = GET_PRIMARY_ADAPTER_WAKEUP(padapter);
	pmlmepriv = &padapter->mlmepriv;

	if (rtw_time_after(rtw_get_current_time() + rtw_ms_to_systime(ips_deffer_ms),
			   pwrpriv->ips_deny_time))
		pwrpriv->ips_deny_time =
			rtw_get_current_time() + rtw_ms_to_systime(ips_deffer_ms);

	if (pwrpriv->ps_processing) {
		while (pwrpriv->ps_processing && rtw_get_passing_time_ms(start) <= 3000)
			rtw_msleep_os(10);
	}

	if (pwrpriv->bInSuspend) {
		while (pwrpriv->bInSuspend &&
		       ((rtw_get_passing_time_ms(start) <= 3000 &&
			 !rtw_is_do_late_resume(pwrpriv)) ||
			(rtw_get_passing_time_ms(start) <= 500 &&
			 rtw_is_do_late_resume(pwrpriv))))
			rtw_msleep_os(10);
	}

	if (pwrpriv->bInSuspend == _TRUE) {
		ret = _FAIL;
		goto exit;
	}

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		ret = _SUCCESS;
		goto exit;
	}

	if (rf_off == pwrpriv->rf_pwrstate) {
		if (_FAIL == ips_leave(padapter)) {
			ret = _FAIL;
			goto exit;
		}
	}

	if (rtw_is_drv_stopped(padapter) || !padapter->bup ||
	    !rtw_is_hw_init_completed(padapter)) {
		RTW_INFO("%s: bup=%d\n", caller, padapter->bup);
		ret = _FALSE;
		goto exit;
	}

exit:
	if (rtw_time_after(rtw_get_current_time() + rtw_ms_to_systime(ips_deffer_ms),
			   pwrpriv->ips_deny_time))
		pwrpriv->ips_deny_time =
			rtw_get_current_time() + rtw_ms_to_systime(ips_deffer_ms);
	return ret;
}
