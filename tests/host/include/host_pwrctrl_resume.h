/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_RESUME_H
#define HOST_PWRCTRL_RESUME_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t bool;

#define _TRUE 1
#define _FALSE 0

struct host_early_suspend {
	void *suspend;
	void *resume;
	int level;
};

struct pwrctrl_priv_resume {
	struct host_early_suspend early_suspend;
	u8 do_late_resume;
	void *resume_work;
	void *rtw_workqueue;
};

struct adapter_resume {
	struct pwrctrl_priv_resume pwrctrlpriv;
};

typedef struct adapter_resume _adapter_resume;
typedef struct adapter_resume *PADAPTER_RESUME;

#define adapter_to_pwrctl_resume(a) (&(a)->pwrctrlpriv)

struct host_pwrctrl_resume_trace {
	u32 register_calls;
	u32 unregister_calls;
	u32 resume_process_calls;
	u32 queue_work_calls;
	u32 lock_suspend_calls;
	u32 unlock_suspend_calls;
};

void host_pwrctrl_resume_reset_trace(void);
struct host_pwrctrl_resume_trace *host_pwrctrl_resume_get_trace(void);

void host_early_suspend_cb(void *h);
void host_late_resume_cb(void *h);

u8 rtw_is_earlysuspend_registered(struct pwrctrl_priv_resume *pwrpriv);
u8 rtw_is_do_late_resume(struct pwrctrl_priv_resume *pwrpriv);
void rtw_set_do_late_resume(struct pwrctrl_priv_resume *pwrpriv, u8 enable);
void rtw_register_early_suspend(struct pwrctrl_priv_resume *pwrpriv);
void rtw_unregister_early_suspend(struct pwrctrl_priv_resume *pwrpriv);
void rtw_resume_in_workqueue(struct pwrctrl_priv_resume *pwrpriv);

#endif /* HOST_PWRCTRL_RESUME_H */
