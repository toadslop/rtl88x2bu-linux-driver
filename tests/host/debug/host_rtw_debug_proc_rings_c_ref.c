// SPDX-License-Identifier: GPL-2.0
/* L1 stub proc exports for W3-233 PR1 rx/tx ring proc getters. */

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
