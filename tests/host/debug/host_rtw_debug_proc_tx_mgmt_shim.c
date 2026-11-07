// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-221 tx mgmt procs (kernel + L2 Rust oracle). */

#include "host_rtw_debug_proc_tx_mgmt_types.h"

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *seq = m;

	if (!seq)
		return NULL;
	return rtw_netdev_priv((struct net_device *)seq->private);
}

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

void rtw_rust_debug_proc_emit_tx_sa_query(void *sel)
{
	RTW_PRINT_SEL(sel, "%s\n", "proc_get_tx_sa_query");
}

long rtw_rust_debug_proc_set_tx_sa_query(void *adapter, const void *buffer,
					 size_t count)
{
	return host_proc_set_tx_sa_query(adapter, buffer, count);
}

void rtw_rust_debug_proc_emit_tx_deauth(void *sel)
{
	RTW_PRINT_SEL(sel, "%s\n", "proc_get_tx_deauth");
}

long rtw_rust_debug_proc_set_tx_deauth(void *adapter, const void *buffer,
				       size_t count)
{
	return host_proc_set_tx_deauth(adapter, buffer, count);
}

void rtw_rust_debug_proc_emit_tx_auth(void *sel)
{
	RTW_PRINT_SEL(sel, "%s\n", "proc_get_tx_auth");
}

long rtw_rust_debug_proc_set_tx_auth(void *adapter, const void *buffer,
				     size_t count)
{
	return host_proc_set_tx_auth(adapter, buffer, count);
}
