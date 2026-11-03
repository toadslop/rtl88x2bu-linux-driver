// SPDX-License-Identifier: GPL-2.0
/* W3-202 C oracle: mi timer cancel + intf lifecycle buddy control. */
#include <stddef.h>
#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define BIT(x) (1U << (x))

struct dvobj_priv {
	u8 iface_nums;
	struct _adapter *padapters[4];
};

struct _adapter {
	u8 iface_id;
	u8 adapter_up;
	struct dvobj_priv *dvobj;
};

struct mi_timer_intf_trace {
	u8 cancel_timer[4];
	u8 reset_drv[4];
	u8 intf_start[4];
	u8 intf_stop[4];
};

struct mi_timer_intf_trace g_mi_timer_intf_trace;

void mi_timer_intf_trace_reset(void)
{
	memset(&g_mi_timer_intf_trace, 0, sizeof(g_mi_timer_intf_trace));
}

static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

struct _adapter *mi_timer_intf_test_adapter(int idx)
{
	if (idx < 0 || idx >= 4)
		return NULL;
	return &g_if[idx];
}

void mi_timer_intf_test_setup(unsigned int up_bm)
{
	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_if[i].adapter_up = (up_bm & BIT(i)) ? 1 : 0;
		g_dv.padapters[i] = &g_if[i];
	}
}

void rtw_cancel_all_timer(struct _adapter *adapter)
{
	g_mi_timer_intf_trace.cancel_timer[adapter->iface_id]++;
}

void rtw_reset_drv_sw(struct _adapter *adapter)
{
	g_mi_timer_intf_trace.reset_drv[adapter->iface_id]++;
}

void rtw_intf_start(struct _adapter *adapter)
{
	g_mi_timer_intf_trace.intf_start[adapter->iface_id]++;
}

void rtw_intf_stop(struct _adapter *adapter)
{
	g_mi_timer_intf_trace.intf_stop[adapter->iface_id]++;
}

typedef u8 (*mi_op)(struct _adapter *, void *);

static u8 mi_process(struct _adapter *pad, int ex_self, void *data, mi_op op)
{
	u8 ret = 0;

	for (int i = 0; i < pad->dvobj->iface_nums; i++) {
		struct _adapter *iface = pad->dvobj->padapters[i];

		if (!iface || !iface->adapter_up)
			continue;
		if (ex_self && iface == pad)
			continue;
		if (op && op(iface, data) == _TRUE)
			ret++;
	}
	return ret;
}

static u8 mi_process_no_schk(struct _adapter *pad, int ex_self, void *data, mi_op op)
{
	u8 ret = 0;

	for (int i = 0; i < pad->dvobj->iface_nums; i++) {
		struct _adapter *iface = pad->dvobj->padapters[i];

		if (!iface)
			continue;
		if (ex_self && iface == pad)
			continue;
		if (op && op(iface, data) == _TRUE)
			ret++;
	}
	return ret;
}

static u8 op_cancel_timer(struct _adapter *a, void *data)
{
	(void)data;
	rtw_cancel_all_timer(a);
	return _TRUE;
}

static u8 op_reset_drv(struct _adapter *a, void *data)
{
	(void)data;
	rtw_reset_drv_sw(a);
	return _TRUE;
}

static u8 op_intf_start(struct _adapter *a, void *data)
{
	(void)data;
	rtw_intf_start(a);
	return _TRUE;
}

static u8 op_intf_stop(struct _adapter *a, void *data)
{
	(void)data;
	rtw_intf_stop(a);
	return _TRUE;
}

void rtw_mi_cancel_all_timer(struct _adapter *adapter)
{
	mi_process(adapter, 0, NULL, op_cancel_timer);
}

void rtw_mi_buddy_cancel_all_timer(struct _adapter *adapter)
{
	mi_process(adapter, 1, NULL, op_cancel_timer);
}

void rtw_mi_reset_drv_sw(struct _adapter *adapter)
{
	mi_process_no_schk(adapter, 0, NULL, op_reset_drv);
}

void rtw_mi_buddy_reset_drv_sw(struct _adapter *adapter)
{
	mi_process_no_schk(adapter, 1, NULL, op_reset_drv);
}

void rtw_mi_intf_start(struct _adapter *adapter)
{
	mi_process(adapter, 0, NULL, op_intf_start);
}

void rtw_mi_buddy_intf_start(struct _adapter *adapter)
{
	mi_process(adapter, 1, NULL, op_intf_start);
}

void rtw_mi_intf_stop(struct _adapter *adapter)
{
	mi_process(adapter, 0, NULL, op_intf_stop);
}

void rtw_mi_buddy_intf_stop(struct _adapter *adapter)
{
	mi_process(adapter, 1, NULL, op_intf_stop);
}

u8 mi_oracle_call_timer_intf(struct _adapter *pad, int fn_id)
{
	switch (fn_id) {
	case 0:
		return mi_process(pad, 0, NULL, op_cancel_timer);
	case 1:
		return mi_process(pad, 1, NULL, op_cancel_timer);
	case 2:
		return mi_process_no_schk(pad, 0, NULL, op_reset_drv);
	case 3:
		return mi_process_no_schk(pad, 1, NULL, op_reset_drv);
	case 4:
		return mi_process(pad, 0, NULL, op_intf_start);
	case 5:
		return mi_process(pad, 1, NULL, op_intf_start);
	case 6:
		return mi_process(pad, 0, NULL, op_intf_stop);
	case 7:
		return mi_process(pad, 1, NULL, op_intf_stop);
	default:
		return 0xff;
	}
}
