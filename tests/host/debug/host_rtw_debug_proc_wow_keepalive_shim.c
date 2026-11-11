// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-235 L2 — emit logic mirrors debug_proc_wow_keepalive_c_oracle.c */

#include "host_rtw_debug_proc_wow_keepalive_types.h"

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

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
