// SPDX-License-Identifier: GPL-2.0
/* C oracle proc exports for W3-232 PCI conf space getters (host L2). */

#include "host_rtw_debug_proc_pci_types.h"

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

int proc_get_pci_aspm(struct seq_file *m, void *v)
{
	struct _adapter *padapter = rtw_rust_debug_proc_adapter(m);

	(void)v;
	return rtw_rust_debug_proc_get_pci_aspm(m, padapter);
}
