/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_PS_PROCESSOR_H
#define HOST_PWRCTRL_PS_PROCESSOR_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

#define _TRUE 1
#define _FALSE 0

#define IPS_NONE 0
#define rf_on 0
#define rf_off 2

typedef int _pwrlock;

struct mlme_priv {
	u32 fw_state;
};

struct pwrctrl_priv {
	_pwrlock lock;
	u32 ps_deny;
	u8 bInSuspend;
	u8 ps_processing;
	u8 ips_mode_req;
	u8 rf_pwrstate;
	u8 change_rfpwrstate;
	u32 pwr_state_check_cnts;
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
	struct mlme_priv mlmepriv;
};

typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);

u32 rtw_ps_deny_get(PADAPTER padapter);
void rtw_ps_processor(PADAPTER padapter);

struct host_pwrctrl_ps_processor_trace {
	u32 ips_enter;
	u32 pwr_timer_set;
};

void host_pwrctrl_ps_proc_reset_trace(void);
struct host_pwrctrl_ps_processor_trace *host_pwrctrl_ps_proc_get_trace(void);
void host_pwrctrl_ps_proc_set_unassoc_idle(u8 idle);

#endif /* HOST_PWRCTRL_PS_PROCESSOR_H */
