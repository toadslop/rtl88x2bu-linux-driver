// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-225 LPS chk-by-TP (host L2). */

#include "host_rtw_debug_proc_lps_tp_types.h"

int proc_get_lps_chk_tp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)v;
	rtw_rust_debug_proc_emit_lps_chk_tp(m, adapter);
	return 0;
}

ssize_t proc_set_lps_chk_tp(void *file, const void *buffer, size_t count,
			    long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	return rtw_rust_debug_proc_set_lps_chk_tp(adapter, buffer, count);
}
