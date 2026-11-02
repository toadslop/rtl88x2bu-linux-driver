// SPDX-License-Identifier: GPL-2.0
/* L1 reference: timer cancel + intf lifecycle leaf for W3-202 (eight rtw_mi_* exports). */
#include <stddef.h>

typedef unsigned char u8;

#define _TRUE 1

struct dvobj_priv {
	int iface_nums;
	struct _adapter *padapters[4];
};

struct _adapter {
	u8 adapter_up;
	struct dvobj_priv *dvobj;
};

static void stub_cancel_timer(struct _adapter *a)
{
	(void)a;
}

static void stub_reset_drv(struct _adapter *a)
{
	(void)a;
}

static void stub_intf_start(struct _adapter *a)
{
	(void)a;
}

static void stub_intf_stop(struct _adapter *a)
{
	(void)a;
}

static u8 mi_process(struct _adapter *pad, int ex, u8 (*op)(struct _adapter *, void *), void *data)
{
	u8 ret = 0;
	int i;

	for (i = 0; i < pad->dvobj->iface_nums; i++) {
		struct _adapter *iface = pad->dvobj->padapters[i];

		if (!iface || !iface->adapter_up)
			continue;
		if (ex && iface == pad)
			continue;
		if (op && op(iface, data) == _TRUE)
			ret++;
	}
	return ret;
}

static u8 mi_process_no_schk(struct _adapter *pad, int ex, u8 (*op)(struct _adapter *, void *),
			     void *data)
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

static u8 op_cancel_timer(struct _adapter *a, void *data)
{
	(void)data;
	stub_cancel_timer(a);
	return _TRUE;
}

static u8 op_reset_drv(struct _adapter *a, void *data)
{
	(void)data;
	stub_reset_drv(a);
	return _TRUE;
}

static u8 op_intf_start(struct _adapter *a, void *data)
{
	(void)data;
	stub_intf_start(a);
	return _TRUE;
}

static u8 op_intf_stop(struct _adapter *a, void *data)
{
	(void)data;
	stub_intf_stop(a);
	return _TRUE;
}

void rtw_mi_cancel_all_timer(struct _adapter *adapter)
{
	mi_process(adapter, 0, op_cancel_timer, NULL);
}

void rtw_mi_buddy_cancel_all_timer(struct _adapter *adapter)
{
	mi_process(adapter, 1, op_cancel_timer, NULL);
}

void rtw_mi_reset_drv_sw(struct _adapter *adapter)
{
	mi_process_no_schk(adapter, 0, op_reset_drv, NULL);
}

void rtw_mi_buddy_reset_drv_sw(struct _adapter *adapter)
{
	mi_process_no_schk(adapter, 1, op_reset_drv, NULL);
}

void rtw_mi_intf_start(struct _adapter *adapter)
{
	mi_process(adapter, 0, op_intf_start, NULL);
}

void rtw_mi_buddy_intf_start(struct _adapter *adapter)
{
	mi_process(adapter, 1, op_intf_start, NULL);
}

void rtw_mi_intf_stop(struct _adapter *adapter)
{
	mi_process(adapter, 0, op_intf_stop, NULL);
}

void rtw_mi_buddy_intf_stop(struct _adapter *adapter)
{
	mi_process(adapter, 1, op_intf_stop, NULL);
}
