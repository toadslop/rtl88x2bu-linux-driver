/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_WAKEUP_H
#define HOST_PWRCTRL_WAKEUP_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define WIFI_ASOC_STATE 0x00000001

typedef u32 systime;

struct mlme_priv_wakeup {
	u32 fw_state;
};

struct pwrctrl_priv_wakeup {
	u8 ps_processing;
	u8 bInSuspend;
	u8 do_late_resume;
	u8 rf_pwrstate;
	systime ips_deny_time;
};

struct dvobj_priv_wakeup {
	struct _adapter_wakeup *padapters[4];
};

struct _adapter_wakeup {
	struct dvobj_priv_wakeup *dvobj;
	struct mlme_priv_wakeup mlmepriv;
	struct pwrctrl_priv_wakeup pwrctrlpriv;
	u8 bup;
};

typedef struct _adapter_wakeup _adapter_wakeup;
typedef _adapter_wakeup *PADAPTER_WAKEUP;

#define adapter_to_pwrctl_wakeup(a) (&(a)->pwrctrlpriv)
#define GET_PRIMARY_ADAPTER_WAKEUP(a) ((a)->dvobj->padapters[0])

struct host_pwrctrl_wakeup_trace {
	u32 leave_all_ps;
	u32 ips_leave_calls;
	s32 ips_leave_result;
};

void host_pwrctrl_wakeup_reset_trace(void);
struct host_pwrctrl_wakeup_trace *host_pwrctrl_wakeup_get_trace(void);
void host_pwrctrl_wakeup_set_ips_leave_result(s32 result);
void host_pwrctrl_wakeup_set_passing_ms(u32 ms);
void host_pwrctrl_wakeup_set_time(u32 t);

void rtw_set_ips_deny(PADAPTER_WAKEUP padapter, u32 ms);
int _rtw_pwr_wakeup(PADAPTER_WAKEUP padapter, u32 ips_deffer_ms, const char *caller);

#endif /* HOST_PWRCTRL_WAKEUP_H */
