/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_SET_PS_H
#define HOST_PWRCTRL_SET_PS_H
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
#define _TRUE 1
#define PS_MODE_ACTIVE 0
#define PM_Card_Disable 10
#define LPS_PG 2
#define LPS_PG_INFO_CFG 0
#define HOST_HW_VAR_LPS_PG_HANDLE 100
typedef int _pwrlock;
struct pwrctrl_priv {
	u8 pwr_mode, smart_ps, bcn_ant_mode;
	u32 lps_leave_cnts;
	u8 lps_level, wowlan_mode, bFwCurrentInPSMode;
	_pwrlock lock;
};
struct adapter { struct pwrctrl_priv pwrctrlpriv; };
typedef struct adapter _adapter;
typedef struct adapter *PADAPTER;
#define adapter_to_pwrctl(a) (&(a)->pwrctrlpriv)
struct host_pwrctrl_set_ps_trace {
	u8 rsvd_page_calls, default_port_calls;
	u32 hwreg_count;
	u8 hwreg_var[4], hwreg_val[4];
};
void host_pwrctrl_set_ps_reset_trace(void);
struct host_pwrctrl_set_ps_trace *host_pwrctrl_set_ps_get_trace(void);
void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);
void rtw_set_ps_mode(PADAPTER padapter, u8 ps_mode, u8 smart_ps, u8 bcn_ant_mode,
		     const char *msg);
#endif
