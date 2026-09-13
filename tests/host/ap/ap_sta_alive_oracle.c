// SPDX-License-Identifier: GPL-2.0
/* Host oracle for chk_sta_is_alive — keep in sync with core/rtw_ap.c. */
#include "host_ap_sta_alive_types.h"

u8 chk_sta_is_alive(struct sta_info *psta)
{
	u8 ret = _FALSE;

	if ((psta->sta_stats.last_rx_data_pkts + psta->sta_stats.last_rx_ctrl_pkts) ==
	    (psta->sta_stats.rx_data_pkts + psta->sta_stats.rx_ctrl_pkts))
		;
	else
		ret = _TRUE;

	sta_update_last_rx_pkts(psta);

	return ret;
}
