/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_IPS_H
#define HOST_PWRCTRL_IPS_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

typedef int _lock;
typedef _lock _pwrlock;

#define rf_off 2
#define rf_on 0
#define IPS_LEVEL_2 2
#define IPS_NONE 0

struct pwrctrl_priv {
	_pwrlock lock;
	u32 bips_processing;
	u8 ips_mode;
	u8 ips_mode_req;
	u32 ips_enter_cnts;
	u32 ips_leave_cnts;
	u8 change_rfpwrstate;
	u8 bpower_saving;
	u8 bkeepfwalive;
	u8 rf_pwrstate;
};

struct _adapter {
	u8 adapter_type;
	struct pwrctrl_priv pwrctrlpriv;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
#define is_primary_adapter(a) ((a)->adapter_type == 0)

void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);

void _ips_enter(PADAPTER padapter);
void ips_enter(PADAPTER padapter);
int _ips_leave(PADAPTER padapter);
int ips_leave(PADAPTER padapter);

struct host_pwrctrl_ips_trace {
	u32 ips_pwr_down;
	u32 ips_pwr_up;
	int ips_pwr_up_result;
	u32 btcoex_notify;
	u8 last_btcoex_mode;
	u8 is_primary;
};

void host_pwrctrl_ips_reset_trace(void);
void host_pwrctrl_ips_set_pwr_up_result(int result);
struct host_pwrctrl_ips_trace *host_pwrctrl_ips_get_trace(void);

#endif /* HOST_PWRCTRL_IPS_H */
