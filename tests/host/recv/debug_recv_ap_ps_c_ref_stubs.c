// SPDX-License-Identifier: GPL-2.0
/* Vacuous stubs so L1 C reference .o exports only process_* symbols. */

#include "host_recv_ap_ps_types.h"

void stop_sta_xmit(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
}

void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
}

void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
}

int issue_qos_nulldata(_adapter *padapter, unsigned char *da, u16 tid, u8 ps,
		       int try_cnt, int wait_ms)
{
	(void)padapter;
	(void)da;
	(void)tid;
	(void)ps;
	(void)try_cnt;
	(void)wait_ms;
	return 0;
}
