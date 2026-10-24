// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-163 L1 (proc ps_dbg_info get/set). */

#include <stddef.h>

typedef struct seq_file seq_file;
typedef struct file file;

long proc_set_ps_dbg_info(void *file, const void *buffer, unsigned long count,
			  long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_ps_dbg_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
