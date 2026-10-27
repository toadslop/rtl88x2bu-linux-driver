/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_ALIVE_H
#define HOST_PWRCTRL_ALIVE_H

#include <stdint.h>

typedef uint8_t u8;
typedef int32_t s32;
typedef uint32_t u32;

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define PS_RF_OFF (1u << 1)
#define PS_STATE_S2 PS_RF_OFF

#define XMIT_ALIVE (1u << 0)
#define RECV_ALIVE (1u << 1)
#define CMD_ALIVE (1u << 2)
#define EVT_ALIVE (1u << 3)
#define LPS_ALIVE (1u << 5)

typedef int _pwrlock;

struct pwrctrl_priv {
	_pwrlock lock;
	u32 alives;
	u8 bFwCurrentInPSMode;
	u8 cpwm;
	u8 rpwm;
};

struct adapter {
	struct pwrctrl_priv pwrctrlpriv;
};

typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;

#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)

void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);

s32 rtw_register_task_alive(PADAPTER padapter, u32 task);
s32 rtw_register_tx_alive(PADAPTER padapter);
s32 rtw_register_cmd_alive(PADAPTER padapter);
s32 rtw_register_rx_alive(PADAPTER padapter);
s32 rtw_register_evt_alive(PADAPTER padapter);

struct host_pwrctrl_alive_trace {
	u32 set_rpwm_calls;
	u8 last_set_rpwm_pslv;
};

void host_pwrctrl_alive_reset_trace(void);
struct host_pwrctrl_alive_trace *host_pwrctrl_alive_get_trace(void);
void host_pwrctrl_alive_set_rpwm_raises_cpwm(u8 v);

#endif /* HOST_PWRCTRL_ALIVE_H */
