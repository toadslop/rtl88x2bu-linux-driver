// SPDX-License-Identifier: GPL-2.0
/* Stub linker symbols for W3-145 L1 (proc mlme/rf leaf getters). */

#include <stddef.h>

typedef struct seq_file seq_file;

int proc_get_fwstate(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_mlmext_state(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_qos_option(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_ht_option(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_rf_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_sec_info(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_roam_flags(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

int proc_get_roam_param(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_roam_flags(void *file, const void *buffer, unsigned long count,
			 long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

long proc_set_roam_param(void *file, const void *buffer, unsigned long count,
			 long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

long proc_set_roam_tgt_addr(void *file, const void *buffer,
			    unsigned long count, long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_defs_param(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_defs_param(void *file, const void *buffer, unsigned long count,
			 long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

long proc_set_write_reg(void *file, const void *buffer, unsigned long count,
			long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}

int proc_get_read_reg(seq_file *m, void *v)
{
	(void)m;
	(void)v;
	return 0;
}

long proc_set_read_reg(void *file, const void *buffer, unsigned long count,
		       long long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
