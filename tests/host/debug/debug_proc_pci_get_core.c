// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-232 PCI conf proc getters (host L2 + L1 ref). */

#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_pci_types.h"

u16 host_pci_pciehdr_offset = 64;
u32 host_pci_conf_dword = 0x04030201;
struct pci_dev *host_pci_read_dev;
u32 host_pci_linkcap;
u16 host_pci_linkctl;
u8 host_pci_l1off_cap;
u8 host_pci_l1off_nic;
u8 host_pci_dbi_byte[0x800];

void host_pci_get_test_reset(void)
{
	host_pci_pciehdr_offset = 64;
	host_pci_conf_dword = 0x04030201;
	host_pci_read_dev = NULL;
	host_pci_linkcap = 0;
	host_pci_linkctl = 0;
	host_pci_l1off_cap = 0;
	host_pci_l1off_nic = 0;
	memset(host_pci_dbi_byte, 0, sizeof(host_pci_dbi_byte));
}

void host_pci_read_config_dword(struct pci_dev *dev, u32 reg, u32 *val)
{
	u32 lnkcap = host_pci_pciehdr_offset + PCI_EXP_LNKCAP;
	u32 lnkctl = host_pci_pciehdr_offset + PCI_EXP_LNKCTL;

	host_pci_read_dev = dev;
	if (!val)
		return;
	if (reg == lnkcap) {
		*val = host_pci_linkcap;
		return;
	}
	if (reg == lnkctl) {
		*val = host_pci_linkctl;
		return;
	}
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

void host_pci_read_config_word(struct pci_dev *dev, u32 reg, u16 *val)
{
	u32 lnkctl = host_pci_pciehdr_offset + PCI_EXP_LNKCTL;

	(void)dev;
	if (!val)
		return;
	if (reg == lnkctl)
		*val = host_pci_linkctl;
	else
		*val = (u16)(host_pci_conf_dword & 0xffff);
}

u8 rtw_hal_pci_l1off_capability(struct _adapter *padapter)
{
	(void)padapter;
	return host_pci_l1off_cap;
}

u8 rtw_hal_pci_l1off_nic_support(struct _adapter *padapter)
{
	(void)padapter;
	return host_pci_l1off_nic;
}

u8 rtw_hal_pci_dbi_read(struct _adapter *padapter, u16 addr)
{
	(void)padapter;
	return addr < sizeof(host_pci_dbi_byte) ? host_pci_dbi_byte[addr] : 0;
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

int rtw_rust_debug_proc_get_pci_aspm(void *sel, void *adapter)
{
	struct _adapter *padapter = adapter;
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct pci_priv *pcipriv = &pdvobjpriv->pcipriv;
	u8 tmp8 = 0;
	u16 tmp16 = 0;
	u32 tmp32 = 0;
	u8 l1_idle = 0;

	if (!padapter)
		return 0;

	host_pci_pciehdr_offset = pcipriv->pciehdr_offset;
	RTW_PRINT_SEL(sel, "***** ASPM Capability *****\n");
	host_pci_read_config_dword(pdvobjpriv->ppcidev,
				  pcipriv->pciehdr_offset + PCI_EXP_LNKCAP,
				  &tmp32);
	RTW_PRINT_SEL(sel, "CLK REQ:\t%s\n",
		      (tmp32 & PCI_EXP_LNKCAP_CLKPM) ? "Enable" : "Disable");
	RTW_PRINT_SEL(sel, "ASPM L1:\t%s\n",
		      (tmp32 & BIT11) ? "Enable" : "Disable");
	tmp8 = rtw_hal_pci_l1off_capability(padapter);
	RTW_PRINT_SEL(sel, "ASPM L1OFF:\t%s\n", tmp8 ? "Enable" : "Disable");
	host_pci_read_config_word(pdvobjpriv->ppcidev,
				  pcipriv->pciehdr_offset + PCI_EXP_LNKCTL,
				  &tmp16);
	RTW_PRINT_SEL(sel, "ASPM L1:\t%s\n",
		      (tmp16 & BIT1) ? "Enable" : "Disable");
	tmp8 = rtw_hal_pci_dbi_read(padapter, 0x70f);
	l1_idle = tmp8 & 0x38;
	RTW_PRINT_SEL(sel, "ASPM L1 Idel Time: 0x%x\n", l1_idle >> 3);
	return 0;
}
