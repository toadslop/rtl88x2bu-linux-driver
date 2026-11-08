// SPDX-License-Identifier: GPL-2.0
/* L1 stub symbols for W3-226 static SMPS proc handlers. */

#include <stddef.h>

typedef unsigned long size_t;
typedef long ssize_t;
typedef long loff_t;

struct seq_file;
struct file;

int proc_get_smps(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_smps(struct file *f, const void *buf, size_t cnt, loff_t *pos,
		      void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}
