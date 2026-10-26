/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_FW_PS_H
#define HOST_PWRCTRL_FW_PS_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

typedef int _lock;
typedef _lock _pwrlock;

#define BIT_LPS_STATUS (1 << 3)

struct registry_priv {
	u8 check_fw_ps;
};

struct debug_priv {
	u32 dbg_poll_fail_cnt;
};

struct dvobj_priv {
	struct debug_priv drv_dbg;
};

struct pwrctrl_priv {
	_pwrlock check_32k_lock;
};

struct _adapter {
	struct registry_priv registrypriv;
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv pwrctrlpriv;
	u8 bSurpriseRemoved;
	u8 bDriverStopped;
	u8 hw_init_completed;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);

void host_pwrctrl_fw_ps_set_hal_state(u16 state);
int rtw_fw_ps_state(PADAPTER padapter);

#endif /* HOST_PWRCTRL_FW_PS_H */
