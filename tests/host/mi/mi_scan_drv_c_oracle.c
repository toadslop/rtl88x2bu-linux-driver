// SPDX-License-Identifier: GPL-2.0
/* W3-199 C oracle: mi scan abort + drv thread control (host subset). */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define BIT(x) (1U << (x))

struct dvobj_priv {
	u8 iface_nums;
	struct _adapter *padapters[4];
};

struct _adapter {
	u8 iface_id;
	u8 adapter_up;
	u8 start_fail;
	struct dvobj_priv *dvobj;
};

struct mi_scan_drv_trace {
	u8 scan_wait[4];
	u8 scan_nowait[4];
	u8 start_drv[4];
	u8 stop_drv[4];
};

struct mi_scan_drv_trace g_mi_scan_drv_trace;

void mi_scan_drv_trace_reset(void)
{
	memset(&g_mi_scan_drv_trace, 0, sizeof(g_mi_scan_drv_trace));
}

static struct dvobj_priv g_dv;
static struct _adapter g_if[4];

struct _adapter *mi_scan_drv_test_adapter(int idx)
{
	if (idx < 0 || idx >= 4)
		return NULL;
	return &g_if[idx];
}

void mi_scan_drv_test_setup(unsigned int up_bm, unsigned int start_fail_bm)
{
	memset(&g_dv, 0, sizeof(g_dv));
	memset(g_if, 0, sizeof(g_if));
	g_dv.iface_nums = 4;
	for (int i = 0; i < 4; i++) {
		g_if[i].iface_id = (u8)i;
		g_if[i].dvobj = &g_dv;
		g_if[i].adapter_up = (up_bm & BIT(i)) ? 1 : 0;
		g_if[i].start_fail = (start_fail_bm & BIT(i)) ? 1 : 0;
		g_dv.padapters[i] = &g_if[i];
	}
}

void rtw_scan_abort(struct _adapter *adapter)
{
	g_mi_scan_drv_trace.scan_wait[adapter->iface_id]++;
}

void rtw_scan_abort_no_wait(struct _adapter *adapter)
{
	g_mi_scan_drv_trace.scan_nowait[adapter->iface_id]++;
}

u32 rtw_start_drv_threads(struct _adapter *adapter)
{
	g_mi_scan_drv_trace.start_drv[adapter->iface_id]++;
	return adapter->start_fail ? _FAIL : _SUCCESS;
}

void rtw_stop_drv_threads(struct _adapter *adapter)
{
	g_mi_scan_drv_trace.stop_drv[adapter->iface_id]++;
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

static u8 op_scan_abort(struct _adapter *a, void *data)
{
	bool bwait = *(bool *)data;

	if (bwait)
		rtw_scan_abort(a);
	else
		rtw_scan_abort_no_wait(a);
	return _TRUE;
}

void rtw_mi_scan_abort(struct _adapter *adapter, bool bwait)
{
	bool in_data = bwait;

	mi_process(adapter, 0, &in_data, op_scan_abort);
}

void rtw_mi_buddy_scan_abort(struct _adapter *adapter, bool bwait)
{
	bool in_data = bwait;

	mi_process(adapter, 1, &in_data, op_scan_abort);
}

u8 mi_oracle_scan_abort(struct _adapter *adapter, bool buddy, bool bwait)
{
	bool in_data = bwait;

	return mi_process(adapter, buddy ? 1 : 0, &in_data, op_scan_abort);
}

static u32 start_drv_all(struct _adapter *adapter, bool exclude_self)
{
	u32 status = _SUCCESS;

	for (int i = 0; i < adapter->dvobj->iface_nums; i++) {
		struct _adapter *iface = adapter->dvobj->padapters[i];

		if (!iface)
			continue;
		if (exclude_self && iface == adapter)
			continue;
		if (rtw_start_drv_threads(iface) == _FAIL) {
			status = _FAIL;
			break;
		}
	}
	return status;
}

u32 rtw_mi_start_drv_threads(struct _adapter *adapter)
{
	return start_drv_all(adapter, 0);
}

u32 rtw_mi_buddy_start_drv_threads(struct _adapter *adapter)
{
	return start_drv_all(adapter, 1);
}

static void stop_drv_all(struct _adapter *adapter, bool exclude_self)
{
	for (int i = 0; i < adapter->dvobj->iface_nums; i++) {
		struct _adapter *iface = adapter->dvobj->padapters[i];

		if (!iface)
			continue;
		if (exclude_self && iface == adapter)
			continue;
		rtw_stop_drv_threads(iface);
	}
}

void rtw_mi_stop_drv_threads(struct _adapter *adapter)
{
	stop_drv_all(adapter, 0);
}

void rtw_mi_buddy_stop_drv_threads(struct _adapter *adapter)
{
	stop_drv_all(adapter, 1);
}
