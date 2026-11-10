// SPDX-License-Identifier: GPL-2.0
/* L1 stub symbols for W3-230 (proc + shim names only). */

#include <stddef.h>

typedef unsigned long size_t;
typedef long ssize_t;
typedef long loff_t;

struct seq_file;
struct file;
struct _adapter;

int proc_get_iqk_info(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_iqk(struct file *f, const void *buf, size_t cnt,
		     loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_lck_info(struct seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

ssize_t proc_set_lck(struct file *f, const void *buf, size_t cnt,
		     loff_t *pos, void *data)
{
	(void)f;
	(void)buf;
	(void)cnt;
	(void)pos;
	(void)data;
	return 0;
}

ssize_t rtw_rust_debug_proc_set_iqk(struct _adapter *padapter,
				    const void *buffer, size_t count)
{
	(void)padapter;
	(void)buffer;
	(void)count;
	return 0;
}

ssize_t rtw_rust_debug_proc_set_lck(struct _adapter *padapter,
				    const void *buffer, size_t count)
{
	(void)padapter;
	(void)buffer;
	(void)count;
	return 0;
}
