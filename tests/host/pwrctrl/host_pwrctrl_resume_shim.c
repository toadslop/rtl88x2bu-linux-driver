/* SPDX-License-Identifier: GPL-2.0 */
/* Host C oracle for W3-194 early suspend + resume workqueue helpers. */

#include "host_pwrctrl_resume.h"

#include <stddef.h>
#include <string.h>

static struct host_pwrctrl_resume_trace g_trace;

void host_pwrctrl_resume_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
}

struct host_pwrctrl_resume_trace *host_pwrctrl_resume_get_trace(void)
{
	return &g_trace;
}

static int host_resume_process(PADAPTER_RESUME padapter)
{
	(void)padapter;
	g_trace.resume_process_calls++;
	return 0;
}

static void host_resume_lock_suspend(void)
{
	g_trace.lock_suspend_calls++;
}

static void host_queue_work(void *wq, void *work)
{
	(void)work;
	if (wq)
		g_trace.queue_work_calls++;
}

#define EARLY_SUSPEND_LEVEL_BLANK_SCREEN 50

void host_early_suspend_cb(void *h)
{
	struct pwrctrl_priv_resume *pwr =
		(struct pwrctrl_priv_resume *)((char *)h -
					       offsetof(struct pwrctrl_priv_resume,
							early_suspend));

	pwr->do_late_resume = _FALSE;
}

void host_late_resume_cb(void *h)
{
	struct pwrctrl_priv_resume *pwr =
		(struct pwrctrl_priv_resume *)((char *)h -
					       offsetof(struct pwrctrl_priv_resume,
							early_suspend));
	_adapter_resume adapter;

	if (pwr->do_late_resume) {
		pwr->do_late_resume = _FALSE;
		memset(&adapter, 0, sizeof(adapter));
		adapter.pwrctrlpriv = *pwr;
		host_resume_process(&adapter);
	}
}

static void host_register_early_suspend(struct pwrctrl_priv_resume *pwr)
{
	g_trace.register_calls++;
	pwr->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN - 20;
	pwr->early_suspend.suspend = (void *)host_early_suspend_cb;
	pwr->early_suspend.resume = (void *)host_late_resume_cb;
}

static void host_unregister_early_suspend(struct pwrctrl_priv_resume *pwr)
{
	g_trace.unregister_calls++;
	pwr->do_late_resume = _FALSE;
	if (pwr->early_suspend.suspend)
		(void)0;
	pwr->early_suspend.suspend = NULL;
	pwr->early_suspend.resume = NULL;
}

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
u8 rtw_is_earlysuspend_registered(struct pwrctrl_priv_resume *pwrpriv)
{
	return pwrpriv->early_suspend.suspend ? _TRUE : _FALSE;
}

u8 rtw_is_do_late_resume(struct pwrctrl_priv_resume *pwrpriv)
{
	return pwrpriv->do_late_resume ? _TRUE : _FALSE;
}

void rtw_set_do_late_resume(struct pwrctrl_priv_resume *pwrpriv, u8 enable)
{
	pwrpriv->do_late_resume = enable ? _TRUE : _FALSE;
}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
void rtw_register_early_suspend(struct pwrctrl_priv_resume *pwrpriv)
{
	host_register_early_suspend(pwrpriv);
}

void rtw_unregister_early_suspend(struct pwrctrl_priv_resume *pwrpriv)
{
	host_unregister_early_suspend(pwrpriv);
}
#endif

#ifdef CONFIG_RESUME_IN_WORKQUEUE
void rtw_resume_in_workqueue(struct pwrctrl_priv_resume *pwrpriv)
{
	host_resume_lock_suspend();
	host_queue_work(pwrpriv->rtw_workqueue, pwrpriv->resume_work);
}
#endif
