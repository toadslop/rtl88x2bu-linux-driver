// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-235 proc_dump_wow_keep_alive_info (host L2 + L1 c_ref). */

#include "host_rtw_debug_proc_wow_keepalive_types.h"

void rtw_rust_debug_proc_emit_wow_keep_alive_info(void *sel, void *padapter)
{
	struct _adapter *adapter = padapter;
	struct pwrctrl_priv *pwrpriv;
	int i;

	if (!adapter)
		return;

	pwrpriv = adapter_to_pwrctl(adapter);

	RTW_PRINT_SEL(sel, "wowlan_keep_alive_mode: %d\n",
		      pwrpriv->wowlan_keep_alive_mode);
	RTW_PRINT_SEL(sel, "LocKeepAlive: %d\n", pwrpriv->keep_alive_pattern_loc);
	RTW_PRINT_SEL(sel, "keep_alive_pattern_len: %d\n",
		      pwrpriv->keep_alive_pattern_len);
	RTW_PRINT_SEL(sel, "keep_alive_pattern= \n");
	for (i = 0; i < (int)pwrpriv->keep_alive_pattern_len; i++) {
		RTW_PRINT_SEL(sel, "[0x%x] ", pwrpriv->keep_alive_pattern[i]);
		if (i % 8 == 7)
			RTW_PRINT_SEL(sel, "\n");
	}
	RTW_PRINT_SEL(sel, "\n");
	RTW_PRINT_SEL(sel, " wowlan_keep_alive_period= %d ms\n",
		      pwrpriv->wowlan_keep_alive_period * 100);
	RTW_PRINT_SEL(sel, " wowlan_keep_alive_retry_counter= %d\n",
		      pwrpriv->wowlan_keep_alive_retry_counter);
	RTW_PRINT_SEL(sel, " wowlan_keep_alive_retry_interval= %d ms\n",
		      pwrpriv->wowlan_keep_alive_retry_interval * 100);
}

#ifndef RUST_RTW_DEBUG_PROC_WOW_KEEPALIVE_ORACLE
int proc_dump_wow_keep_alive_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;

	if (padapter)
		rtw_rust_debug_proc_emit_wow_keep_alive_info(m, padapter);
	return 0;
}
#endif /* !RUST_RTW_DEBUG_PROC_WOW_KEEPALIVE_ORACLE */
