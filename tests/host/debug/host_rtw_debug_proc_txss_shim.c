// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-224 TXSS proc L2 Rust oracle. */

#include "host_rtw_debug_proc_txss_types.h"

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
