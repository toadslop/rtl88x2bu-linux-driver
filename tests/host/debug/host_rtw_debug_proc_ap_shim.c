// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-158 L1 (proc ap_info getter). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_ap_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
