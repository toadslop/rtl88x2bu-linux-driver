// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-134 MSR HALMAC L2 harness. */

#include <string.h>
#include "host_hal_com_msr_types.h"

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;
static u8 g_mbid_entries;
static u8 g_last_get_port, g_last_set_port, g_last_set_type, g_stored_type;

void host_msr_reset_trace(void)
{
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.dvobj = &g_dvobj;
	g_mbid_entries = 0;
	g_last_get_port = g_last_set_port = 0xff;
	g_stored_type = _HW_STATE_NOLINK_;
}

void host_msr_set_hw_port(u8 port) { g_adapter.hw_port = port; }
void host_msr_set_mbid_entry_num(u8 n) { g_mbid_entries = n; }
struct _adapter *host_msr_adapter(void) { return &g_adapter; }
u8 host_msr_last_get_port(void) { return g_last_get_port; }
u8 host_msr_last_set_port(void) { return g_last_set_port; }
u8 host_msr_last_get_type(void) { return g_stored_type; }
u8 host_msr_last_set_type(void) { return g_last_set_type; }
void host_msr_clear_calls(void) { g_last_get_port = g_last_set_port = 0xff; }

u8 rtw_get_mbid_cam_entry_num(struct _adapter *adapter)
{
	(void)adapter;
	return g_mbid_entries;
}

int rtw_halmac_get_network_type(struct dvobj_priv *d, int hwport, u8 *type)
{
	(void)d;
	g_last_get_port = (u8)hwport;
	if (type)
		*type = g_stored_type;
	return 0;
}

int rtw_halmac_set_network_type(struct dvobj_priv *d, int hwport, u8 type)
{
	(void)d;
	g_last_set_port = (u8)hwport;
	g_last_set_type = type;
	g_stored_type = type;
	return 0;
}