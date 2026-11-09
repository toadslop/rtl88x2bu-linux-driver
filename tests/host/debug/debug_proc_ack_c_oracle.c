// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-227 ACK timeout (host L2). */

#include "host_rtw_debug_proc_ack_types.h"

int proc_get_ack_timeout(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)v;
	if (!padapter)
		return 0;
	rtw_rust_debug_proc_emit_ack_timeout(m, padapter);
	return 0;
}

ssize_t proc_set_ack_timeout(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	if (!padapter)
		return -EFAULT;
	return rtw_rust_debug_proc_set_ack_timeout(padapter, buffer, count);
}
