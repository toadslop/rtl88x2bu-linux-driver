// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for W3-184 PM ioctl setters — mirrors core/rtw_pwrctrl.c. */

#include "host_pwrctrl_pm.h"

#define RTW_INFO(fmt, ...) ((void)0)

static struct host_pwrctrl_pm_trace g_trace;
static s32 g_pwr_wakeup_result = _SUCCESS;

void host_pwrctrl_pm_reset_trace(void)
{
	g_trace.leave_all_ps = 0;
	g_trace.ips_mode_req_calls = 0;
	g_trace.last_ips_mode_req = 0;
	g_trace.pwr_wakeup_calls = 0;
}

struct host_pwrctrl_pm_trace *host_pwrctrl_pm_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_pm_set_pwr_wakeup_result(s32 result)
{
	g_pwr_wakeup_result = result;
}

static void LeaveAllPowerSaveMode(PADAPTER_PM padapter)
{
	(void)padapter;
	g_trace.leave_all_ps++;
}

static void rtw_ips_mode_req(struct pwrctrl_priv_pm *pwrctl, u8 ips_mode)
{
	pwrctl->ips_mode_req = ips_mode;
	g_trace.ips_mode_req_calls++;
	g_trace.last_ips_mode_req = ips_mode;
}

static u8 rtw_is_surprise_removed(PADAPTER_PM padapter)
{
	return padapter->bSurpriseRemoved;
}

static s32 rtw_pwr_wakeup(PADAPTER_PM padapter)
{
	(void)padapter;
	g_trace.pwr_wakeup_calls++;
	return g_pwr_wakeup_result;
}

int rtw_pm_set_lps(PADAPTER_PM padapter, u8 mode)
{
	struct pwrctrl_priv_pm *pwrctrlpriv = adapter_to_pwrctl_pm(padapter);

	if (mode < PS_MODE_NUM) {
		if (pwrctrlpriv->power_mgnt != mode) {
			if (PS_MODE_ACTIVE == mode)
				LeaveAllPowerSaveMode(padapter);
			else
				pwrctrlpriv->LpsIdleCount = 2;
			pwrctrlpriv->power_mgnt = mode;
			pwrctrlpriv->bLeisurePs =
				(PS_MODE_ACTIVE != pwrctrlpriv->power_mgnt) ? _TRUE : _FALSE;
		}
	} else
		return -22;

	return 0;
}

int rtw_pm_set_lps_level(PADAPTER_PM padapter, u8 level)
{
	struct pwrctrl_priv_pm *pwrctrlpriv = adapter_to_pwrctl_pm(padapter);

	if (level < LPS_LEVEL_MAX) {
		if (pwrctrlpriv->lps_level != level)
			pwrctrlpriv->lps_level = level;
	} else
		return -22;

	return 0;
}

int rtw_pm_set_ips(PADAPTER_PM padapter, u8 mode)
{
	struct pwrctrl_priv_pm *pwrctrlpriv = adapter_to_pwrctl_pm(padapter);

	if (mode == IPS_NORMAL || mode == IPS_LEVEL_2) {
		rtw_ips_mode_req(pwrctrlpriv, mode);
		RTW_INFO("%s %s\n", __func__, mode == IPS_NORMAL ? "IPS_NORMAL" : "IPS_LEVEL_2");
		return 0;
	} else if (mode == IPS_NONE) {
		rtw_ips_mode_req(pwrctrlpriv, mode);
		RTW_INFO("%s %s\n", __func__, "IPS_NONE");
		if (!rtw_is_surprise_removed(padapter) && (_FAIL == rtw_pwr_wakeup(padapter)))
			return -14;
	} else
		return -22;
	return 0;
}
