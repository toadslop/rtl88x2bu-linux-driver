/* SPDX-License-Identifier: GPL-2.0 */
#include "host_pwrctrl_set_ps.h"
#include <string.h>
static struct host_pwrctrl_set_ps_trace g_trace;
static u8 g_sys_clkr_in_32k;
static systime g_time;
void host_pwrctrl_set_ps_reset_trace(void) { memset(&g_trace, 0, sizeof(g_trace)); }
struct host_pwrctrl_set_ps_trace *host_pwrctrl_set_ps_get_trace(void) { return &g_trace; }
void host_pwrctrl_set_ps_reset_hal(void) { g_sys_clkr_in_32k = 0; g_time = 0; }
void host_pwrctrl_set_ps_set_sys_clkr_in_32k(u8 v) { g_sys_clkr_in_32k = v ? 1 : 0; }
void _enter_pwrlock(_pwrlock *plock) { (void)plock; }
void _exit_pwrlock(_pwrlock *plock) { (void)plock; }
#ifdef CONFIG_LPS_PG
static void record_hw(u8 var, u8 val)
{
	u32 n = g_trace.hwreg_count;
	if (n >= 4)
		return;
	g_trace.hwreg_var[n] = var;
	g_trace.hwreg_val[n] = val;
	g_trace.hwreg_count++;
}
static void rtw_hal_set_hwreg(PADAPTER a, u8 var, u8 *pval)
{
	(void)a;
	record_hw(var, *pval);
}
#endif
static void rtw_set_ps_rsvd_page(PADAPTER a) { (void)a; g_trace.rsvd_page_calls++; }
static void rtw_set_default_port_id(PADAPTER a) { (void)a; g_trace.default_port_calls++; }
static void rtw_set_rpwm(PADAPTER a, u8 v) { (void)a; g_trace.rpwm_val = v; }
static void rtw_exec_lps(PADAPTER a, u8 m)
{
	(void)a;
	g_trace.exec_lps_called = 1;
	g_trace.exec_lps_ps_mode = m;
}
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN) || defined(CONFIG_P2P_WOWLAN)
static void rtw_hal_get_hwreg(PADAPTER a, u8 var, u8 *pval)
{
	(void)a;
	*pval = (var == HOST_HW_VAR_SYS_CLKR && g_sys_clkr_in_32k) ? (1U << 4) : 0;
}
static void rtw_usleep_os(int us) { (void)us; g_time += 25; }
#endif
void rtw_set_ps_mode(PADAPTER padapter, u8 ps_mode, u8 smart_ps, u8 bcn_ant_mode,
		     const char *msg)
{
	struct pwrctrl_priv *p = adapter_to_pwrctl(padapter);
#ifdef CONFIG_LPS_PG
	u8 lps_pg_hdl_id;
#endif
	(void)msg;
	if (ps_mode > PM_Card_Disable)
		return;
	if (p->pwr_mode == ps_mode) {
		if (ps_mode == PS_MODE_ACTIVE)
			return;
#ifndef CONFIG_BT_COEXIST
		if (p->smart_ps == smart_ps && p->bcn_ant_mode == bcn_ant_mode)
			return;
#else
	(void)smart_ps;
	(void)bcn_ant_mode;
#endif
	}
#ifdef CONFIG_FW_MULTI_PORT_SUPPORT
	if (ps_mode != PS_MODE_ACTIVE) {
		rtw_set_ps_rsvd_page(padapter);
		rtw_set_default_port_id(padapter);
	}
#endif
#ifdef CONFIG_LPS_PG
	if ((ps_mode != PS_MODE_ACTIVE) && (p->lps_level == LPS_PG) &&
	    (p->wowlan_mode != _TRUE)) {
		lps_pg_hdl_id = LPS_PG_INFO_CFG;
		rtw_hal_set_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE, &lps_pg_hdl_id);
	}
#endif
#ifdef CONFIG_LPS_LCLK
	_enter_pwrlock(&p->lock);
	if (ps_mode == PS_MODE_ACTIVE) {
		p->lps_leave_cnts++;
		p->pwr_mode = ps_mode;
		rtw_set_rpwm(padapter, PS_STATE_S4);
#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN) || defined(CONFIG_P2P_WOWLAN)
		if (p->wowlan_mode == _TRUE) {
			systime t0 = g_time;
			u8 val8;

			do {
				rtw_hal_get_hwreg(padapter, HOST_HW_VAR_SYS_CLKR, &val8);
				if (!(val8 & (1U << 4)))
					break;
				if (g_time - t0 > 20) {
					g_trace.wow_leave_fail_cnt++;
					break;
				}
				rtw_usleep_os(100);
			} while (1);
		}
#endif
#ifdef CONFIG_LPS_PG
		if (p->lps_level == LPS_PG) {
			lps_pg_hdl_id = LPS_PG_REDLEMEM;
			rtw_hal_set_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE,
					  &lps_pg_hdl_id);
		}
#endif
#ifdef CONFIG_WOWLAN
		if (p->wowlan_mode == _TRUE)
			rtw_hal_set_hwreg(padapter, HOST_HW_VAR_H2C_INACTIVE_IPS, &ps_mode);
#endif
		rtw_exec_lps(padapter, ps_mode);
#ifdef CONFIG_LPS_PG
		if (p->lps_level == LPS_PG) {
			lps_pg_hdl_id = LPS_PG_PHYDM_EN;
			rtw_hal_set_hwreg(padapter, HOST_HW_VAR_LPS_PG_HANDLE,
					  &lps_pg_hdl_id);
		}
#endif
		p->bFwCurrentInPSMode = _FALSE;
	}
	g_trace.lps_leave_cnts = p->lps_leave_cnts;
	g_trace.pwr_mode = p->pwr_mode;
	g_trace.b_fw_current_in_ps = p->bFwCurrentInPSMode;
	_exit_pwrlock(&p->lock);
#endif
}
