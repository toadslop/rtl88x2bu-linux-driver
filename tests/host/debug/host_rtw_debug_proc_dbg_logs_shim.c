// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-164 L1 (proc dbg counter log getters). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rx_logs(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_tx_logs(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_int_logs(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
