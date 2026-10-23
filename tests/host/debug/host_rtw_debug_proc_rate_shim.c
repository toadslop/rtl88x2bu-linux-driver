// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-160 PR1 L1 (proc_get_rate_ctl). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rate_ctl(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
