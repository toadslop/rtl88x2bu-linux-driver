// SPDX-License-Identifier: GPL-2.0
/* Host fixtures for W3-46 recv LLC/ethhdr/BMC L2 oracles. */

#include "host_recv_types.h"

void rtw_rframe_set_os_pkt(union recv_frame *rframe)
{
	(void)rframe;
}

sint rtw_linked_check(_adapter *adapter)
{
	return adapter->host_linked ? _TRUE : _FALSE;
}
