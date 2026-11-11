// SPDX-License-Identifier: GPL-2.0
/* L1 stub proc exports for W3-234 RTKM prealloc buffer info proc. */

#include <stddef.h>

struct seq_file;

int proc_get_rtkm_info(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
