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

s32 rtw_get_passing_time_ms(systime start)
{
	/* Match _rtw_get_passing_time_ms: signed elapsed via jiffies subtraction. */
	return (s32)(host_current_time - start);
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

void *rtw_rust_hal_com_adapter_to_dvobj(void *adapter)
{
	return adapter_to_dvobj(adapter);
}

systime rtw_rust_hal_com_get_current_time(void)
{
	return rtw_get_current_time();
}

u32 rtw_rust_hal_com_get_passing_time_ms(systime start)
{
	return rtw_get_passing_time_ms(start);
}

int rtw_rust_hal_com_time_after(systime a, systime b)
{
	return rtw_time_after(a, b);
}

systime rtw_rust_hal_com_ms_to_systime(int ms)
{
	return rtw_ms_to_systime(ms);
}

systime rtw_rust_hal_com_tsf_periodic_etime(struct dvobj_priv *dvobj)
{
	return dvobj->periodic_tsf_update_etime;
}

void rtw_rust_hal_com_tsf_periodic_set_etime(struct dvobj_priv *dvobj, systime etime)
{
	dvobj->periodic_tsf_update_etime = etime;
}

u8 rtw_rust_hal_com_tsf_rf_offch_state(struct dvobj_priv *dvobj)
{
	return dvobj->rf_ctl.offch_state;
}

int rtw_rust_hal_com_tsf_iface_nums(struct dvobj_priv *dvobj)
{
	return dvobj->iface_nums;
}

struct _adapter *rtw_rust_hal_com_tsf_iface_at(struct dvobj_priv *dvobj, int i)
{
	return dvobj->padapters[i];
}

u8 rtw_rust_hal_com_tsf_ext_required(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_required;
}

systime rtw_rust_hal_com_tsf_ext_pause_stime(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_pause_stime;
}

u8 rtw_rust_hal_com_tsf_ext_pause_factor(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_pause_factor;
}

u8 rtw_rust_hal_com_tsf_ext_restore_factor(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.tsf_update_restore_factor;
}

u16 rtw_rust_hal_com_tsf_ext_bcn_interval(void *adapter)
{
	return ((struct _adapter *)adapter)->mlmeextpriv.mlmext_info.bcn_interval;
}

void rtw_rust_hal_com_tsf_periodic_arm_timer(struct dvobj_priv *dvobj, u32 ms)
{
	_set_timer(&dvobj->periodic_tsf_update_end_timer, ms);
}

struct _adapter *rtw_rust_hal_com_dvobj_primary(struct dvobj_priv *dvobj)
{
	return dvobj_get_primary_adapter(dvobj);
}

int rtw_rust_hal_com_dev_surprise(struct dvobj_priv *dvobj)
{
	return dev_is_surprise_removed(dvobj);
}

int rtw_rust_hal_com_dev_stopped(struct dvobj_priv *dvobj)
{
	return dev_is_drv_stopped(dvobj);
}

u8 rtw_rust_hal_com_periodic_tsf_end_cmd(struct _adapter *adapter)
{
	return rtw_periodic_tsf_update_end_cmd(adapter);
}