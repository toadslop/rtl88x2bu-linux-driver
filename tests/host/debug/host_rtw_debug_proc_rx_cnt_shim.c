// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-162 L1 (proc dump_rx_cnt_mode handlers). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rx_cnt_dump(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_rx_cnt_dump(void *file, const void *buffer, unsigned long count,
			  long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
