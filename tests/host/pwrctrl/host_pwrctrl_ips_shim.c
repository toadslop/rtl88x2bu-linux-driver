// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for W3-167 IPS enter/leave (sync with core/rtw_pwrctrl.c). */

#include <string.h>
#include "host_pwrctrl_ips.h"

#define RTW_INFO(fmt, ...) ((void)0)
#define RTW_PRINT(fmt, ...) ((void)0)

static struct host_pwrctrl_ips_trace g_trace;

void host_pwrctrl_ips_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
	g_trace.ips_pwr_up_result = _SUCCESS;
}

void host_pwrctrl_ips_set_pwr_up_result(int result)
{
	g_trace.ips_pwr_up_result = result;
}

struct host_pwrctrl_ips_trace *host_pwrctrl_ips_get_trace(void)
{
	return &g_trace;
}

void _enter_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

void _exit_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

static void rtw_ips_pwr_down(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ips_pwr_down++;
}

static int rtw_ips_pwr_up(PADAPTER padapter)
{
	(void)padapter;
	g_trace.ips_pwr_up++;
	return g_trace.ips_pwr_up_result;
}

static void rtw_btcoex_IpsNotify(PADAPTER padapter, u8 mode)
{
	(void)padapter;
	g_trace.btcoex_notify++;
	g_trace.last_btcoex_mode = mode;
}

void _ips_enter(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	pwrpriv->bips_processing = _TRUE;
	pwrpriv->ips_mode = pwrpriv->ips_mode_req;
	pwrpriv->ips_enter_cnts++;
	RTW_INFO("ips_enter cnts:%u\n", pwrpriv->ips_enter_cnts);

	if (rf_off == pwrpriv->change_rfpwrstate) {
		pwrpriv->bpower_saving = _TRUE;
		RTW_PRINT("nolinked power save enter\n");
		if (pwrpriv->ips_mode == IPS_LEVEL_2)
			pwrpriv->bkeepfwalive = _TRUE;
		rtw_ips_pwr_down(padapter);
		pwrpriv->rf_pwrstate = rf_off;
	}
	pwrpriv->bips_processing = _FALSE;
}

void ips_enter(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	rtw_btcoex_IpsNotify(padapter, pwrpriv->ips_mode_req);
	_enter_pwrlock(&pwrpriv->lock);
	_ips_enter(padapter);
	_exit_pwrlock(&pwrpriv->lock);
}

int _ips_leave(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	int result = _SUCCESS;

	if ((pwrpriv->rf_pwrstate == rf_off) && (!pwrpriv->bips_processing)) {
		pwrpriv->bips_processing = _TRUE;
		pwrpriv->change_rfpwrstate = rf_on;
		pwrpriv->ips_leave_cnts++;
		RTW_INFO("ips_leave cnts:%u\n", pwrpriv->ips_leave_cnts);

		result = rtw_ips_pwr_up(padapter);
		if (result == _SUCCESS)
			pwrpriv->rf_pwrstate = rf_on;

		RTW_PRINT("nolinked power save leave\n");
		pwrpriv->bips_processing = _FALSE;
		pwrpriv->bkeepfwalive = _FALSE;
		pwrpriv->bpower_saving = _FALSE;
	}

	return result;
}

int ips_leave(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	int ret;

	if (!is_primary_adapter(padapter))
		return _SUCCESS;

	_enter_pwrlock(&pwrpriv->lock);
	ret = _ips_leave(padapter);
	_exit_pwrlock(&pwrpriv->lock);

	if (_SUCCESS == ret)
		rtw_btcoex_IpsNotify(padapter, IPS_NONE);

	return ret;
}
