/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_PWRCTRL_SET_PS_H
#define HOST_PWRCTRL_SET_PS_H
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef u32 systime;
#define _TRUE 1
#define _FALSE 0
#define PS_MODE_ACTIVE 0
#define PM_Card_Disable 10
#define LPS_PG 2
#define LPS_PG_INFO_CFG 0
#define LPS_PG_REDLEMEM 1
#define LPS_PG_PHYDM_EN 3
#define PS_STATE_S4 ((1U << 3) | (1U << 2))
#define HOST_HW_VAR_LPS_PG_HANDLE 100
#define HOST_HW_VAR_H2C_INACTIVE_IPS 101
#define HOST_HW_VAR_SYS_CLKR 102
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
	u8 rsvd_page_calls, default_port_calls, rpwm_val, exec_lps_called, exec_lps_ps_mode;
	u8 b_fw_current_in_ps, pwr_mode;
	u32 lps_leave_cnts, hwreg_count, wow_leave_fail_cnt;
	u8 hwreg_var[4], hwreg_val[4];
};
void host_pwrctrl_set_ps_reset_trace(void);
struct host_pwrctrl_set_ps_trace *host_pwrctrl_set_ps_get_trace(void);
void host_pwrctrl_set_ps_reset_hal(void);
void host_pwrctrl_set_ps_set_sys_clkr_in_32k(u8 in_32k);
void _enter_pwrlock(_pwrlock *plock);
void _exit_pwrlock(_pwrlock *plock);
void rtw_set_ps_mode(PADAPTER padapter, u8 ps_mode, u8 smart_ps, u8 bcn_ant_mode,
		     const char *msg);
#endif
