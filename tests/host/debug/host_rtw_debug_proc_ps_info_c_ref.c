// SPDX-License-Identifier: GPL-2.0
/* L1 stub symbols for W3-219 (proc names only — bodies unused by rust-check-symbols). */

#include <stddef.h>

typedef unsigned long size_t;

typedef struct seq_file seq_file;
typedef struct file file;

typedef long ssize_t;
typedef long loff_t;

int proc_get_new_bcn_max(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_new_bcn_max(file *f, const void *buf, size_t cnt,
			     loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_ps_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_ps_info(file *f, const void *buf, size_t cnt,
			 loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_wmmps_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_wmmps_info(file *f, const void *buf, size_t cnt,
			    loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}
