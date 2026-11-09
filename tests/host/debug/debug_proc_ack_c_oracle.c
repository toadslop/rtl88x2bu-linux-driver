// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-227 ACK timeout (host L2). */

#include "host_rtw_debug_proc_ack_types.h"

int proc_get_ack_timeout(struct seq_file *m, void *v)
{
	struct _adapter *padapter;

	(void)v;
	padapter = (_adapter *)rtw_netdev_priv((struct net_device *)m->private);
	if (!padapter)
		return 0;
	rtw_rust_debug_proc_emit_ack_timeout(m, padapter);
	return 0;
}

ssize_t proc_set_ack_timeout(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct _adapter *padapter;

	(void)file;
	(void)pos;
	padapter = (_adapter *)rtw_netdev_priv((struct net_device *)data);
	if (!padapter)
		return -EFAULT;
	return rtw_rust_debug_proc_set_ack_timeout(padapter, buffer, count);
}
