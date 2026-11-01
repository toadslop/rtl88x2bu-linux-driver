// SPDX-License-Identifier: GPL-2.0
/* Host L2 smoke for W3-194 early suspend + resume workqueue helpers. */

#include <stdio.h>
#include <string.h>

#include "host_pwrctrl_resume.h"

static struct adapter_resume g_adapter;

static int invoke_late_resume(struct pwrctrl_priv_resume *p)
{
	void (*resume)(void *);

	if (!p->early_suspend.resume) {
		fputs("FAIL late_resume_cb_missing\n", stderr);
		return 1;
	}
	resume = (void (*)(void *))p->early_suspend.resume;
	resume(&p->early_suspend);
	return 0;
}

static int check_register_and_flags(void)
{
	struct pwrctrl_priv_resume *p = adapter_to_pwrctl_resume(&g_adapter);
	struct host_pwrctrl_resume_trace *tr;

	host_pwrctrl_resume_reset_trace();
	memset(&g_adapter, 0, sizeof(g_adapter));
	p->rtw_workqueue = (void *)1;
	tr = host_pwrctrl_resume_get_trace();

	rtw_register_early_suspend(p);
	if (!rtw_is_earlysuspend_registered(p) || tr->register_calls != 1) {
		fputs("FAIL register\n", stderr);
		return 1;
	}

	rtw_set_do_late_resume(p, _TRUE);
	if (!rtw_is_do_late_resume(p)) {
		fputs("FAIL set_do_late\n", stderr);
		return 1;
	}

	if (invoke_late_resume(p)) {
		return 1;
	}
	if (rtw_is_do_late_resume(p) || tr->resume_process_calls != 1) {
		fputs("FAIL late_resume\n", stderr);
		return 1;
	}

	rtw_unregister_early_suspend(p);
	if (rtw_is_earlysuspend_registered(p) || tr->unregister_calls != 1) {
		fputs("FAIL unregister\n", stderr);
		return 1;
	}

#ifdef CONFIG_RESUME_IN_WORKQUEUE
	rtw_resume_in_workqueue(p);
	if (tr->lock_suspend_calls != 1 || tr->queue_work_calls != 1) {
		fputs("FAIL resume_wq\n", stderr);
		return 1;
	}
#endif

	puts("PASS early_suspend_resume");
	return 0;
}

int main(void)
{
	int bad = check_register_and_flags();

	if (!bad)
		puts("ALL PASS (1 vector)");
	return bad ? 1 : 0;
}
