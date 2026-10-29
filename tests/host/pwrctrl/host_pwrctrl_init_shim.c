/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-189 — rtw_init_pwrctrl_priv part 1. */

#include "host_pwrctrl_init.h"

#include <string.h>

static struct host_pwrctrl_init_trace g_trace;
static systime g_current_time;

void host_pwrctrl_init_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
}

struct host_pwrctrl_init_trace *host_pwrctrl_init_get_trace(void)
{
	return &g_trace;
}

void host_pwrctrl_init_set_current_time(systime t)
{
	g_current_time = t;
}

static void _init_pwrlock(_pwrlock *plock)
{
	(void)plock;
}

static void _init_workitem(void *wi, void (*fn)(void *), void *ctx)
{
	(void)wi;
	(void)fn;
	(void)ctx;
	g_trace.init_workitem_calls++;
}

static void rtw_init_timer(void *timer, PADAPTER padapter, void (*fn)(void *), void *ctx)
{
	(void)timer;
	(void)padapter;
	(void)fn;
	(void)ctx;
	g_trace.init_timer_calls++;
}

static void rtw_hal_set_hwreg(PADAPTER padapter, u8 var, u8 *val)
{
	(void)padapter;
	if (var == HOST_HW_VAR_SET_RPWM)
		g_trace.set_rpwm_hwreg_calls++;
	(void)val;
}

static void *create_singlethread_workqueue(const char *name)
{
	(void)name;
	g_trace.create_wq_calls++;
	return (void *)1;
}

void rtw_init_pwrctrl_priv_part1(PADAPTER padapter)
{
	struct pwrctrl_priv_init *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct registry_priv_init *reg = &padapter->registrypriv;

#if defined(CONFIG_CONCURRENT_MODE)
	if (!is_primary_adapter(padapter)) {
		g_trace.skipped_non_primary = 1;
		return;
	}
#endif

	_init_pwrlock(&pwrctrlpriv->lock);
	_init_pwrlock(&pwrctrlpriv->check_32k_lock);
	pwrctrlpriv->rf_pwrstate = rf_on;
	pwrctrlpriv->ips_enter_cnts = 0;
	pwrctrlpriv->ips_leave_cnts = 0;
	pwrctrlpriv->lps_enter_cnts = 0;
	pwrctrlpriv->lps_leave_cnts = 0;
	pwrctrlpriv->bips_processing = _FALSE;

	pwrctrlpriv->ips_mode = reg->ips_mode;
	pwrctrlpriv->ips_mode_req = reg->ips_mode;
	pwrctrlpriv->ips_deny_time = g_current_time;
	pwrctrlpriv->lps_level = reg->lps_level;

	pwrctrlpriv->pwr_state_check_interval = RTW_PWR_STATE_CHK_INTERVAL;
	pwrctrlpriv->pwr_state_check_cnts = 0;
	pwrctrlpriv->bInSuspend = _FALSE;
	pwrctrlpriv->bkeepfwalive = _FALSE;
	pwrctrlpriv->LpsIdleCount = 0;

	if (reg->mp_mode == 1)
		pwrctrlpriv->power_mgnt = PS_MODE_ACTIVE;
	else
		pwrctrlpriv->power_mgnt = reg->power_mgnt;
	pwrctrlpriv->bLeisurePs =
		(pwrctrlpriv->power_mgnt != PS_MODE_ACTIVE) ? _TRUE : _FALSE;

	pwrctrlpriv->bFwCurrentInPSMode = _FALSE;
	pwrctrlpriv->lps_deny_time = g_current_time;

	pwrctrlpriv->rpwm = 0;
	pwrctrlpriv->cpwm = PS_STATE_S4;

	pwrctrlpriv->pwr_mode = PS_MODE_ACTIVE;
	pwrctrlpriv->smart_ps = reg->smart_ps;
	pwrctrlpriv->bcn_ant_mode = 0;
	pwrctrlpriv->dtim = 0;

	pwrctrlpriv->tog = 0x80;
	pwrctrlpriv->rpwm_retry = 0;

#ifdef CONFIG_LPS_LCLK
	rtw_hal_set_hwreg(padapter, HOST_HW_VAR_SET_RPWM, &pwrctrlpriv->rpwm);
	_init_workitem(&pwrctrlpriv->cpwm_event, NULL, NULL);
	_init_workitem(&pwrctrlpriv->dma_event, NULL, NULL);
#endif

#ifdef CONFIG_LPS_PG
	pwrctrlpriv->lpspg_info.name = "LPSPG_INFO";
#endif

	rtw_init_timer(&pwrctrlpriv->pwr_state_check_timer, padapter, NULL, NULL);

	pwrctrlpriv->wowlan_mode = _FALSE;
	pwrctrlpriv->wowlan_ap_mode = _FALSE;
	pwrctrlpriv->wowlan_p2p_mode = _FALSE;
	pwrctrlpriv->wowlan_in_resume = _FALSE;
	pwrctrlpriv->wowlan_last_wake_reason = 0;

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	_init_workitem(&pwrctrlpriv->resume_work, NULL, NULL);
	pwrctrlpriv->rtw_workqueue = create_singlethread_workqueue("rtw_workqueue");
#endif
}
