// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-160 L1 (proc rate leaf handlers). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_bmc_tx_rate(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_bmc_tx_rate(void *file, const void *buffer, unsigned long count,
			  long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

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

int proc_get_bw_ctl(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_bw_ctl(void *file, const void *buffer, unsigned long count,
		     long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
