// SPDX-License-Identifier: GPL-2.0
/* Shared C oracle for W3-231 PCI conf space proc setters (host L2 + L1 ref). */

#include <stdarg.h>

#include "host_rtw_debug_proc_pci_types.h"

unsigned host_pci_write_count_var;
struct pci_dev *host_pci_write_dev;
u32 host_pci_write_reg;
u8 host_pci_write_val;
char host_pci_log_buf[96];

void host_pci_write_config_byte(struct pci_dev *dev, u32 reg, u8 val)
{
	host_pci_write_count_var++;
	host_pci_write_dev = dev;
	host_pci_write_reg = reg;
	host_pci_write_val = val;
}

void host_rtw_info_line(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(host_pci_log_buf, sizeof(host_pci_log_buf), fmt, ap);
	va_end(ap);
}

#ifdef HOST_PCI_L1_REF
#define HOST_PCI_SCOPE static
#else
#define HOST_PCI_SCOPE
#endif

HOST_PCI_SCOPE ssize_t pci_conf_set_common(struct _adapter *padapter,
						  struct pci_dev *target,
						  const void *buffer,
						  size_t count)
{
	char tmp[32];
	int num;
	u32 reg = 0;
	u32 value = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count < sizeof(tmp))
			tmp[count] = '\0';
		num = sscanf(tmp, "%x %x", &reg, &value);
		if (num != 2) {
			RTW_INFO("invalid parameter!\n");
			return (ssize_t)count;
		}

		if (reg >= 0x1000) {
			RTW_INFO("invalid register!\n");
			return (ssize_t)count;
		}

		if (value > 0xFF) {
			RTW_INFO("invalid value! Only one byte\n");
			return (ssize_t)count;
		}

		RTW_INFO("adapter 0x%p: register 0x%x value 0x%x\n", (void *)padapter,
			 reg, value);

		host_pci_write_config_byte(target, reg, (u8)value);
	}
	return (ssize_t)count;
}

HOST_PCI_SCOPE ssize_t rtw_rust_debug_proc_set_pci_bridge_conf_space(
	struct _adapter *padapter, const void *buffer, size_t count)
{
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct pci_dev *pdev = pdvobjpriv->ppcidev;
	struct pci_dev *bridge_pdev = pdev->bus->self;

	return pci_conf_set_common(padapter, bridge_pdev, buffer, count);
}

HOST_PCI_SCOPE ssize_t rtw_rust_debug_proc_set_pci_conf_space(
	struct _adapter *padapter, const void *buffer, size_t count)
{
	struct dvobj_priv *pdvobjpriv = adapter_to_dvobj(padapter);
	struct pci_dev *pdev = pdvobjpriv->ppcidev;

	return pci_conf_set_common(padapter, pdev, buffer, count);
}
