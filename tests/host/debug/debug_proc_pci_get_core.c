// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-232 PCI conf proc getters (host L2 + L1 ref). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_pci_types.h"

u32 host_pci_conf_dword = 0x04030201;
struct pci_dev *host_pci_read_dev;

void host_pci_get_test_reset(void)
{
	host_pci_conf_dword = 0x04030201;
	host_pci_read_dev = NULL;
}

void host_pci_read_config_dword(struct pci_dev *dev, u32 reg, u32 *val)
{
	(void)reg;

	host_pci_read_dev = dev;
	if (!val)
		return;
	if (!dev) {
		*val = host_pci_conf_dword;
		return;
	}
	/* Vary the first dword by PCI device so bridge vs endpoint is observable. */
	if (dev->id == 2)
		*val = 0x06050403U;
	else
		*val = host_pci_conf_dword;
}

void *rtw_rust_debug_proc_adapter(void *m)
{
	struct seq_file *s = m;

	if (!s)
		return NULL;
	return rtw_netdev_priv((struct net_device *)s->private);
}

static void pci_conf_dump(void *sel, struct pci_dev *target, int use_bridge)
{
	u32 tmp[4];
	u32 i, j;

	if (use_bridge)
		RTW_PRINT_SEL(sel,
			      "\n*****  PCI Host Device Configuration Space*****\n\n");
	else
		RTW_PRINT_SEL(sel,
			      "\n*****  PCI Device Configuration Space *****\n\n");

	for (i = 0; i < 0x1000; i += 0x10) {
		for (j = 0; j < 4; j++)
			host_pci_read_config_dword(target, i + j * 4, tmp + j);

		RTW_PRINT_SEL(sel,
			      "%03x: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			      i, tmp[0] & 0xff, (tmp[0] >> 8) & 0xff,
			      (tmp[0] >> 16) & 0xff, (tmp[0] >> 24) & 0xff,
			      tmp[1] & 0xff, (tmp[1] >> 8) & 0xff,
			      (tmp[1] >> 16) & 0xff, (tmp[1] >> 24) & 0xff,
			      tmp[2] & 0xff, (tmp[2] >> 8) & 0xff,
			      (tmp[2] >> 16) & 0xff, (tmp[2] >> 24) & 0xff,
			      tmp[3] & 0xff, (tmp[3] >> 8) & 0xff,
			      (tmp[3] >> 16) & 0xff, (tmp[3] >> 24) & 0xff);
	}
}

int rtw_rust_debug_proc_get_pci_conf_space(void *sel, void *adapter,
					   int use_bridge)
{
	struct _adapter *padapter = adapter;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct pci_dev *pdev = pdvobjpriv->ppcidev;
	struct pci_dev *target = use_bridge ? pdev->bus->self : pdev;

	if (!padapter || !target)
		return 0;
	pci_conf_dump(sel, target, use_bridge);
	return 0;
}
