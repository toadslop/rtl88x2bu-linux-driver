// SPDX-License-Identifier: GPL-2.0
/* Host L2 FFI for rust/rtw_recv_bh_dbg.rs (W3-171). */

#include <stddef.h>
#include <stdio.h>

#include "host_rtw_recv_bh_dbg_types.h"

void rtw_rust_recv_bh_print_sel(void *sel, const char *line)
{
	if (!sel || !line)
		return;
	fputs(line, (FILE *)sel);
}

size_t rtw_rust_recv_bh_append_ptr(char *buf, size_t buflen, size_t pos, void *p)
{
	int n;

	if (!buf || pos >= buflen)
		return pos;
	n = snprintf(buf + pos, buflen - pos, "%p", p);
	if (n <= 0)
		return pos;
	if ((size_t)n >= buflen - pos)
		return buflen;
	return pos + (size_t)n;
}

void rtw_warn_on(int cond)
{
	(void)cond;
}
