// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-204 L1 (proc rx_ampdu get/set). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_rx_ampdu(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_rx_ampdu(void *file, const void *buffer, unsigned long count,
		     long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_tx_quick_addba_req(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_tx_quick_addba_req(void *file, const void *buffer, unsigned long count,
				 long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
