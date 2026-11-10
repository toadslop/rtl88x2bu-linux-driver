// SPDX-License-Identifier: GPL-2.0
/* L1 stub proc exports for W3-231 PCI conf space setters. */

#include <stddef.h>

typedef unsigned long size_t;
typedef long ssize_t;
typedef long loff_t;

struct file;

ssize_t proc_set_pci_bridge_conf_space(struct file *f, const void *buf,
				       size_t cnt, loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}

ssize_t proc_set_pci_conf_space(struct file *f, const void *buf, size_t cnt,
				loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}
