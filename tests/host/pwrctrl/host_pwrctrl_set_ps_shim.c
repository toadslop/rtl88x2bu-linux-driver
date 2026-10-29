/* SPDX-License-Identifier: GPL-2.0 */
#include "host_pwrctrl_set_ps.h"
#include <string.h>
static struct host_pwrctrl_set_ps_trace g_trace;
void host_pwrctrl_set_ps_reset_trace(void) { memset(&g_trace, 0, sizeof(g_trace)); }
struct host_pwrctrl_set_ps_trace *host_pwrctrl_set_ps_get_trace(void) { return &g_trace; }
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
#endif
static void rtw_set_ps_rsvd_page(PADAPTER a) { (void)a; g_trace.rsvd_page_calls++; }
static void rtw_set_default_port_id(PADAPTER a) { (void)a; g_trace.default_port_calls++; }
#ifdef CONFIG_LPS_PG
static void rtw_hal_set_hwreg(PADAPTER a, u8 var, u8 *pval)
{
	(void)a;
	record_hw(var, *pval);
}
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
}
