// SPDX-License-Identifier: GPL-2.0
/* Host FFI for W3-231 PCI conf proc L2 Rust oracle. */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "host_rtw_debug_proc_pci_types.h"

void *rtw_rust_debug_proc_adapter_from_data(void *data)
{
	return rtw_netdev_priv((struct net_device *)data);
}

long rtw_rust_debug_proc_read_user_line(const void *buffer, unsigned long count,
					char *tmp, unsigned long tmp_len)
{
	if (count < 1)
		return -14;
	if (count > tmp_len) {
		rtw_warn_on(1);
		return -14;
	}
	if (!buffer || !tmp)
		return -14;
	memcpy(tmp, buffer, count);
	if (count < tmp_len)
		tmp[count] = '\0';
	return (long)count;
}

int rtw_rust_debug_proc_sscanf_x32_pair(const unsigned char *tmp,
					unsigned int *first, unsigned int *second)
{
	return sscanf((const char *)tmp, "%x %x", first, second);
}

void rtw_rust_pci_write_config_byte(void *adapter, int use_bridge, unsigned int reg,
				    unsigned char val)
{
	struct _adapter *a = adapter;
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct pci_dev *pdev = d->ppcidev;
	struct pci_dev *target = use_bridge ? pdev->bus->self : pdev;

	(void)adapter;
	host_pci_write_config_byte(target, reg, val);
}

void rtw_rust_debug_proc_log_pci_reg_set(void *adapter, unsigned int reg,
					 unsigned int val)
{
	host_rtw_info_line("adapter 0x%p: register 0x%x value 0x%x\n", adapter, reg,
			   val);
}

void rtw_rust_debug_log_info(const char *line)
{
	if (line)
		host_rtw_info_line("%s", line);
}

void rtw_rust_debug_proc_emit_pci_conf_space(void *sel, void *adapter,
					     int use_bridge)
{
	rtw_rust_debug_proc_get_pci_conf_space(sel, adapter, use_bridge);
}

void rtw_rust_debug_proc_emit_pci_aspm(void *sel, void *adapter)
{
	rtw_rust_debug_proc_get_pci_aspm(sel, adapter);
}
