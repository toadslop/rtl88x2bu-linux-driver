// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-170 dump_regsty_rx_ampdu_size_limit (from core/rtw_debug_rest.c). */

#include "host_rtw_debug_regsty_types.h"

void dump_regsty_rx_ampdu_size_limit(void *sel, struct _adapter *adapter)
{
	struct registry_priv *regsty = adapter_to_regsty(adapter);
	int i;

	RTW_PRINT_SEL(sel, "%-3s %-3s %-3s %-3s %-4s\n"
		, "", "20M", "40M", "80M", "160M");
	for (i = 0; i < 4; i++)
		RTW_PRINT_SEL(sel, "%dSS %3u %3u %3u %4u\n", i + 1
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][0]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][1]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][2]
			, regsty->rx_ampdu_sz_limit_by_nss_bw[i][3]);
}
