// SPDX-License-Identifier: GPL-2.0
/* Host oracle for issue_aka_chk_frame (W3-82 PR10 L2). */
#include "host_ap_aka_chk_types.h"

int issue_aka_chk_frame(_adapter *adapter, struct sta_info *psta)
{
	int ret = _FAIL;
	u8 *target_addr = psta->cmn.mac_addr;

	if (MLME_IS_AP(adapter)) {
		if (psta->state & WIFI_SLEEP_STATE)
			ret = issue_nulldata(adapter, target_addr, 0, 1, 50);
		else
			ret = issue_nulldata(adapter, target_addr, 0, 3, 50);
	}

	return ret;
}
