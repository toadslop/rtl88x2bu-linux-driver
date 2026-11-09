// SPDX-License-Identifier: GPL-2.0
/* L1 stub symbols for W3-227 (proc names only). */

#define HOST_ACK_L1_REF 1
#include "host_rtw_debug_proc_ack_types.h"

#include <stddef.h>

typedef long loff_t;

struct file;

int proc_get_ack_timeout(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_ack_timeout(struct file *f, const void *buf, size_t cnt,
			     loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}
