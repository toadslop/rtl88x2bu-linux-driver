// SPDX-License-Identifier: GPL-2.0
/* W3-168 PR1 L1 stub — proc_set_fwdl_test_case only. */

#include <stddef.h>

long proc_set_fwdl_test_case(void *file, const void *buffer, unsigned long count,
			     long *pos, void *data)
{
	(void)file;
	(void)buffer;
	(void)count;
	(void)pos;
	(void)data;
	return 0;
}
