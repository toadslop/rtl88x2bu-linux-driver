// SPDX-License-Identifier: GPL-2.0
/* Host clock + dvobj fixture for W4-130 periodic TSF L2. */

#include "host_hal_com_tsf_periodic_types.h"
#include <string.h>

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapters[4];
static systime host_current_time;

struct host_hal_com_tsf_periodic_trace host_hal_com_tsf_periodic_trace;

void host_hal_com_tsf_periodic_reset(void)
{
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(g_adapters, 0, sizeof(g_adapters));
	memset(&host_hal_com_tsf_periodic_trace, 0, sizeof(host_hal_com_tsf_periodic_trace));
	host_current_time = 1000;
	g_dvobj.iface_nums = 2;
	g_dvobj.padapters[0] = &g_adapters[0];
	g_dvobj.padapters[1] = &g_adapters[1];
	g_dvobj.padapters[2] = &g_adapters[2];
	g_dvobj.padapters[3] = &g_adapters[3];
}

struct _adapter *host_hal_com_tsf_periodic_adapter(void)
{
	return &g_adapters[0];
}

struct dvobj_priv *host_hal_com_tsf_periodic_dvobj(void)
{
	return &g_dvobj;
}

void host_hal_com_tsf_periodic_set_current_time(systime t)
{
	host_current_time = t;
}

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter)
{
	(void)adapter;
	return &g_dvobj;
}

struct _adapter *dvobj_get_primary_adapter(struct dvobj_priv *dvobj)
{
	return dvobj->padapters[0];
}

systime rtw_get_current_time(void)
{
	return host_current_time;
}

systime rtw_ms_to_systime(int ms)
{
	return (systime)ms;
}

int rtw_time_after(systime a, systime b)
{
	return ((long)(b - a)) < 0;
}

u32 rtw_get_passing_time_ms(systime start)
{
	if (host_current_time >= start)
		return (u32)(host_current_time - start);
	return 0;
}

void _set_timer(struct _timer *t, u32 ms)
{
	host_hal_com_tsf_periodic_trace.set_timer_count++;
	host_hal_com_tsf_periodic_trace.last_timer_ms = ms;
	host_hal_com_tsf_periodic_trace.last_timer_target = t;
}

void rtw_hal_rcr_set_chk_bssid(struct _adapter *adapter, u8 self_action)
{
	host_hal_com_tsf_periodic_trace.rcr_chk_bssid_count++;
	host_hal_com_tsf_periodic_trace.last_rcr_chk_action = self_action;
	host_hal_com_tsf_periodic_trace.last_rcr_adapter = adapter;
}

u8 rtw_periodic_tsf_update_end_cmd(struct _adapter *adapter)
{
	host_hal_com_tsf_periodic_trace.periodic_end_cmd_count++;
	host_hal_com_tsf_periodic_trace.last_end_cmd_adapter = adapter;
	return 0;
}

int dev_is_surprise_removed(struct dvobj_priv *dvobj)
{
	return dvobj->surprise_removed ? 1 : 0;
}

int dev_is_drv_stopped(struct dvobj_priv *dvobj)
{
	return dvobj->drv_stopped ? 1 : 0;
}