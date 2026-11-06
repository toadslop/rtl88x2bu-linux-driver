// SPDX-License-Identifier: GPL-2.0
/* L1 reference stubs for W3-215 proc btcoex handlers (symbol names only). */

#include "host_rtw_debug_proc_btcoex_types.h"

int proc_get_btcoex_dbg(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_btcoex_dbg(void *file, const void *buffer, size_t count,
			     long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_btcoex_info(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}
