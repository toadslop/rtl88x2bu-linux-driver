// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-224 TXSS (host L2). */

#include "host_rtw_debug_proc_txss_types.h"

int proc_get_txss_tp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)v;
	rtw_rust_debug_proc_emit_txss_tp(m, adapter);
	return 0;
}

ssize_t proc_set_txss_tp(void *file, const void *buffer, size_t count,
			 long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	return rtw_rust_debug_proc_set_txss_tp(adapter, buffer, count);
}

#ifdef DBG_CTRL_TXSS
int proc_get_txss_ctrl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)v;
	rtw_rust_debug_proc_emit_txss_ctrl(m, adapter);
	return 0;
}

ssize_t proc_set_txss_ctrl(void *file, const void *buffer, size_t count,
			   long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *adapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	return rtw_rust_debug_proc_set_txss_ctrl(adapter, buffer, count);
}
#endif /* DBG_CTRL_TXSS */
