// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-231 PCI conf space setters (host L2). */

#include "host_rtw_debug_proc_pci_types.h"

ssize_t proc_set_pci_bridge_conf_space(void *file, const void *buffer,
				       size_t count, long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	return rtw_rust_debug_proc_set_pci_bridge_conf_space(padapter, buffer, count);
}

ssize_t proc_set_pci_conf_space(void *file, const void *buffer, size_t count,
				long *pos, void *data)
{
	struct net_device *dev = data;
	struct _adapter *padapter = rtw_netdev_priv(dev);

	(void)file;
	(void)pos;
	return rtw_rust_debug_proc_set_pci_conf_space(padapter, buffer, count);
}

int proc_get_pci_bridge_conf_space(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	return rtw_rust_debug_proc_get_pci_conf_space(m, padapter, 1);
}

int proc_get_pci_conf_space(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	return rtw_rust_debug_proc_get_pci_conf_space(m, padapter, 0);
}

