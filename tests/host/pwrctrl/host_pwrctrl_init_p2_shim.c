/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-190 — init part 2 + rtw_free_pwrctrl_priv. */

#include "host_pwrctrl_init.h"

#include <string.h>

static void _free_pwrlock(_pwrlock *plock)
{
	(void)plock;
	host_pwrctrl_init_get_trace()->free_pwrlock++;
}

static void _cancel_workitem_sync(void *wi)
{
	(void)wi;
	host_pwrctrl_init_get_trace()->cancel_workitem++;
}

static void flush_workqueue(void *wq)
{
	(void)wq;
	host_pwrctrl_init_get_trace()->flush_wq++;
}

static void destroy_workqueue(void *wq)
{
	(void)wq;
	host_pwrctrl_init_get_trace()->destroy_wq++;
}

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
static void rtw_register_early_suspend(struct pwrctrl_priv_init *p)
{
	(void)p;
	host_pwrctrl_init_get_trace()->register_early_suspend++;
}

static void rtw_unregister_early_suspend(struct pwrctrl_priv_init *p)
{
	(void)p;
}
#endif

#ifdef CONFIG_WOWLAN
static void rtw_wow_pattern_sw_reset(PADAPTER padapter)
{
	(void)padapter;
	host_pwrctrl_init_get_trace()->wow_pattern_reset++;
}
#endif

void rtw_init_pwrctrl_priv_part2(PADAPTER padapter)
{
	struct pwrctrl_priv_init *pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct registry_priv_init *reg = &padapter->registrypriv;

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	rtw_register_early_suspend(pwrctrlpriv);
#endif

#ifdef CONFIG_WOWLAN
	pwrctrlpriv->wowlan_power_mgmt = reg->wow_power_mgnt;
	pwrctrlpriv->wowlan_lps_level = reg->wow_lps_level;
	if (reg->wakeup_event & (1U << 1))
		pwrctrlpriv->default_patterns_en = _TRUE;
	else
		pwrctrlpriv->default_patterns_en = _FALSE;
	rtw_wow_pattern_sw_reset(padapter);
	pwrctrlpriv->wowlan_aoac_rpt_loc = 0;
#endif

#ifdef CONFIG_LPS_POFF
	host_pwrctrl_init_get_trace()->lps_poff_init++;
#endif

#ifdef CONFIG_LPS_ACK
	host_pwrctrl_init_get_trace()->lps_ack_mutex_init++;
	pwrctrlpriv->lps_ack_status = -1;
#endif
}

void rtw_free_pwrctrl_priv(PADAPTER adapter)
{
	struct pwrctrl_priv_init *pwrctrlpriv = adapter_to_pwrctl(adapter);

#if defined(CONFIG_CONCURRENT_MODE)
	if (!is_primary_adapter(adapter))
		return;
#endif

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	if (pwrctrlpriv->rtw_workqueue) {
		flush_workqueue(pwrctrlpriv->rtw_workqueue);
		destroy_workqueue(pwrctrlpriv->rtw_workqueue);
		pwrctrlpriv->rtw_workqueue = NULL;
	}
#endif

#ifdef CONFIG_LPS_LCLK
	_cancel_workitem_sync(&pwrctrlpriv->cpwm_event);
	_cancel_workitem_sync(&pwrctrlpriv->dma_event);
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	rtw_unregister_early_suspend(pwrctrlpriv);
#endif

	_free_pwrlock(&pwrctrlpriv->lock);
	_free_pwrlock(&pwrctrlpriv->check_32k_lock);
}
