// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-221 (host L2). */

#include "host_rtw_debug_proc_tx_mgmt_types.h"

ssize_t proc_set_tx_sa_query(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_tx_sa_query(padapter, buffer, count);
}

int proc_get_tx_sa_query(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_deauth(void *file, const void *buffer, size_t count,
			   long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_tx_deauth(padapter, buffer, count);
}

int proc_get_tx_deauth(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}

ssize_t proc_set_tx_auth(void *file, const void *buffer, size_t count,
			 long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;

	return host_proc_set_tx_auth(padapter, buffer, count);
}

int proc_get_tx_auth(struct seq_file *m, void *v)
{
	(void)v;
	RTW_PRINT_SEL(m, "%s\n", __func__);
	return 0;
}
