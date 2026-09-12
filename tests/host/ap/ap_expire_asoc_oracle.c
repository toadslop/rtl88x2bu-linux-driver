// SPDX-License-Identifier: GPL-2.0
/* Host oracle for expire_timeout_chk asoc tick — keep in sync with core/rtw_ap.c. */
#include "host_ap_expire_asoc_types.h"

void expire_timeout_asoc_step(_adapter *padapter, struct sta_info *psta, u8 sta_alive)
{
	if (sta_alive || !psta->expire_to) {
		psta->expire_to = padapter->stapriv.expire_to;
		psta->keep_alive_trycnt = 0;
	} else {
		psta->expire_to--;
	}
}
