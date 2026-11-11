// SPDX-License-Identifier: GPL-2.0
/* L1 stub proc exports for W3-233 TRX ring proc handlers. */

#include <stddef.h>

struct seq_file;

int proc_get_rx_ring(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_tx_ring(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_tx_ring_ext(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

typedef long ssize_t;
typedef long loff_t;
struct file;

ssize_t proc_set_tx_ring_ext(struct file *f, const void *buf, size_t cnt,
			     loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}
