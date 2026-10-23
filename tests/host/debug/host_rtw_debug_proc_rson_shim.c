// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-155 L1 (proc rson_data get/set). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rson_data(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_rson_data(void *file, const void *buffer, unsigned long count,
			long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
