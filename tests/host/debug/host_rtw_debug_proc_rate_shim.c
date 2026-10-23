// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-160 PR1–PR2 L1 (rate_ctl + tx_power_offset). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rate_ctl(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_tx_power_offset(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_tx_power_offset(void *file, const void *buffer,
			      unsigned long count, long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
