// SPDX-License-Identifier: GPL-2.0
/* Kernel + host L2 FFI for rust/rtw_debug_regsty.rs (W3-170). */

#include <stddef.h>

#include "host_rtw_debug_regsty_types.h"

void rtw_rust_debug_print_sel(void *sel, const char *line)
{
	if (!sel || !line)
		return;
	fputs(line, (FILE *)sel);
}

u8 *rtw_rust_debug_regsty_rx_ampdu_sz_limit_by_nss_bw(void *adapter)
{
	if (!adapter)
		return NULL;
	return adapter_to_regsty((struct _adapter *)adapter)
		->rx_ampdu_sz_limit_by_nss_bw[0];
}
