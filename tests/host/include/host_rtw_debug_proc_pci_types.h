/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 types for W3-231 PCI config space proc setters (CONFIG_PCI_HCI). */
#ifndef HOST_RTW_DEBUG_PROC_PCI_TYPES_H
#define HOST_RTW_DEBUG_PROC_PCI_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef long ssize_t;

#ifndef EFAULT
#define EFAULT 14
#endif

struct pci_bus {
	struct pci_dev *self;
};

struct pci_dev {
	struct pci_bus *bus;
	u8 id;
};

struct dvobj_priv {
	struct pci_dev *ppcidev;
};

struct seq_file {
	void *private;
	FILE *out;
};

#define RTW_PRINT_SEL(sel, fmt, ...)                                           \
	fprintf(((struct seq_file *)(sel))->out ? ((struct seq_file *)(sel))->out \
					       : stdout,                           \
		fmt, ##__VA_ARGS__)

extern u32 host_pci_conf_dword;

void host_pci_get_test_reset(void);
void host_pci_read_config_dword(struct pci_dev *dev, u32 reg, u32 *val);

void *rtw_rust_debug_proc_adapter(void *m);
int rtw_rust_debug_proc_get_pci_conf_space(void *sel, void *adapter,
					   int use_bridge);

int proc_get_pci_conf_space(struct seq_file *m, void *v);
int proc_get_pci_bridge_conf_space(struct seq_file *m, void *v);

struct _adapter {
	struct dvobj_priv dvobjpriv;
};

struct net_device {
	void *priv;
};

extern unsigned host_pci_write_count_var;
extern struct pci_dev *host_pci_write_dev;
extern u32 host_pci_write_reg;
extern u8 host_pci_write_val;
extern char host_pci_log_buf[96];

static inline void host_pci_write_reset(void)
{
	host_pci_write_count_var = 0;
	host_pci_write_dev = NULL;
	host_pci_write_reg = 0;
	host_pci_write_val = 0;
	host_pci_log_buf[0] = '\0';
}

static inline unsigned host_pci_write_count(void)
{
	return host_pci_write_count_var;
}

static inline void host_pci_write_get(u32 *reg, u8 *val, struct pci_dev **dev)
{
	if (reg)
		*reg = host_pci_write_reg;
	if (val)
		*val = host_pci_write_val;
	if (dev)
		*dev = host_pci_write_dev;
}

static inline const char *host_pci_last_log_line(void)
{
	return host_pci_log_buf;
}

void host_pci_write_config_byte(struct pci_dev *dev, u32 reg, u8 val);
void host_rtw_info_line(const char *fmt, ...);

#define RTW_INFO(fmt, ...) host_rtw_info_line(fmt, ##__VA_ARGS__)

static inline void rtw_warn_on(int cond)
{
	(void)cond;
}

static inline void *rtw_netdev_priv(struct net_device *dev)
{
	return dev ? dev->priv : NULL;
}

#define adapter_to_dvobj(a) (&(a)->dvobjpriv)

#define copy_from_user(dst, src, n) ((void)memcpy((dst), (src), (n)), 0)

#ifndef HOST_PCI_L1_REF
ssize_t rtw_rust_debug_proc_set_pci_conf_space(struct _adapter *adapter,
					       const void *buffer, size_t count);
ssize_t rtw_rust_debug_proc_set_pci_bridge_conf_space(struct _adapter *adapter,
						      const void *buffer,
						      size_t count);
#endif /* HOST_PCI_L1_REF */

#endif /* HOST_RTW_DEBUG_PROC_PCI_TYPES_H */
