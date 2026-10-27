/* SPDX-License-Identifier: GPL-2.0 */
/* Host stubs for W3-177 AP PS recv hooks. */

#include "host_recv_ap_ps_types.h"

#include <string.h>

static struct host_recv_ap_ps_trace g_trace;

void host_recv_ap_ps_reset_trace(void)
{
	memset(&g_trace, 0, sizeof(g_trace));
}

struct host_recv_ap_ps_trace *host_recv_ap_ps_get_trace(void)
{
	return &g_trace;
}

void stop_sta_xmit(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
	g_trace.stop_sta_xmit++;
}

void wakeup_sta_to_xmit(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
	g_trace.wakeup_sta_to_xmit++;
}

void xmit_delivery_enabled_frames(_adapter *padapter, struct sta_info_host *psta)
{
	(void)padapter;
	(void)psta;
	g_trace.xmit_delivery++;
}

int issue_qos_nulldata(_adapter *padapter, unsigned char *da, u16 tid, u8 ps,
		       int try_cnt, int wait_ms)
{
	(void)padapter;
	(void)da;
	(void)try_cnt;
	(void)wait_ms;
	g_trace.issue_qos_null++;
	g_trace.last_qos_tid = tid;
	g_trace.last_qos_ps = ps;
	return 0;
}
