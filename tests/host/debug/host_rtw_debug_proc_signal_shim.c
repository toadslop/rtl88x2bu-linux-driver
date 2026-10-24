// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-166 L1 (proc rx_signal get/set). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rx_signal(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_rx_signal(void *file, const void *buffer, unsigned long count,
			long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
