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
