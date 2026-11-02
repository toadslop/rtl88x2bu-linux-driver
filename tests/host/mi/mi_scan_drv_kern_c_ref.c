// SPDX-License-Identifier: GPL-2.0
/* L1 reference: scan abort + drv thread leaf for W3-199 (six rtw_mi_* exports). */
#include <stddef.h>

typedef unsigned char u8;
typedef unsigned int u32;

#define _TRUE 1
#define _SUCCESS 1
#define _FAIL 0

struct dvobj_priv {
	int iface_nums;
	struct _adapter *padapters[4];
};

struct _adapter {
	struct dvobj_priv *dvobj;
};

static void stub_scan_abort(struct _adapter *a)
{
	(void)a;
}

static void stub_scan_abort_no_wait(struct _adapter *a)
{
	(void)a;
}

static u32 stub_start_drv(struct _adapter *a)
{
	(void)a;
	return _SUCCESS;
}

static void stub_stop_drv(struct _adapter *a)
{
	(void)a;
}

static u8 mi_process(struct _adapter *pad, int ex, u8 (*op)(struct _adapter *, void *), void *data)
{
	u8 ret = 0;
	int i;

	for (i = 0; i < pad->dvobj->iface_nums; i++) {
		struct _adapter *iface = pad->dvobj->padapters[i];

		if (!iface)
			continue;
		if (ex && iface == pad)
			continue;
		if (op && op(iface, data) == _TRUE)
			ret++;
	}
	return ret;
}

static u8 op_scan_abort(struct _adapter *a, void *data)
{
	u8 bwait = *(u8 *)data;

	if (bwait)
		stub_scan_abort(a);
	else
		stub_scan_abort_no_wait(a);
	return _TRUE;
}

static u32 start_drv_all(struct _adapter *adapter, int exclude_self)
{
	u32 status = _SUCCESS;
	int i;

	for (i = 0; i < adapter->dvobj->iface_nums; i++) {
		struct _adapter *iface = adapter->dvobj->padapters[i];

		if (!iface)
			continue;
		if (exclude_self && iface == adapter)
			continue;
		if (stub_start_drv(iface) == _FAIL) {
			status = _FAIL;
			break;
		}
	}
	return status;
}

static void stop_drv_all(struct _adapter *adapter, int exclude_self)
{
	int i;

	for (i = 0; i < adapter->dvobj->iface_nums; i++) {
		struct _adapter *iface = adapter->dvobj->padapters[i];

		if (!iface)
			continue;
		if (exclude_self && iface == adapter)
			continue;
		stub_stop_drv(iface);
	}
}

void rtw_mi_scan_abort(struct _adapter *adapter, u8 bwait)
{
	u8 in_data = bwait;

	mi_process(adapter, 0, op_scan_abort, &in_data);
}

void rtw_mi_buddy_scan_abort(struct _adapter *adapter, u8 bwait)
{
	u8 in_data = bwait;

	mi_process(adapter, 1, op_scan_abort, &in_data);
}

u32 rtw_mi_start_drv_threads(struct _adapter *adapter)
{
	return start_drv_all(adapter, 0);
}

u32 rtw_mi_buddy_start_drv_threads(struct _adapter *adapter)
{
	return start_drv_all(adapter, 1);
}

void rtw_mi_stop_drv_threads(struct _adapter *adapter)
{
	stop_drv_all(adapter, 0);
}

void rtw_mi_buddy_stop_drv_threads(struct _adapter *adapter)
{
	stop_drv_all(adapter, 1);
}
