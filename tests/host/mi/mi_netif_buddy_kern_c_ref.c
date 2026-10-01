// SPDX-License-Identifier: GPL-2.0
/* L1 reference: C netif buddy leaf for W3-123 PR3 (twelve rtw_mi_* exports). */
#include <stddef.h>
typedef unsigned char u8;

#define _TRUE 1
#define _FALSE 0

struct net_device;
struct dvobj_priv {
	int iface_nums;
	struct _adapter *padapters[4];
};
struct _adapter {
	struct net_device *pnetdev;
	struct dvobj_priv *dvobj;
	u8 adapter_up;
};

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

static u8 op_caroff(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}
static u8 op_caron(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}
static u8 op_stop(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}
static u8 op_wake(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}
static u8 op_carr_on(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}
static u8 op_carr_off(struct _adapter *a, void *d)
{
	(void)d;
	(void)a;
	return _TRUE;
}

#define MI_NETIF_PAIR(self_fn, buddy_fn, op)                                       \
	u8 self_fn(struct _adapter *p)                                             \
	{                                                                          \
		return mi_process(p, _FALSE, op, NULL);                            \
	}                                                                          \
	u8 buddy_fn(struct _adapter *p)                                            \
	{                                                                          \
		return mi_process(p, _TRUE, op, NULL);                             \
	}

MI_NETIF_PAIR(rtw_mi_netif_caroff_qstop, rtw_mi_buddy_netif_caroff_qstop, op_caroff);
MI_NETIF_PAIR(rtw_mi_netif_caron_qstart, rtw_mi_buddy_netif_caron_qstart, op_caron);
MI_NETIF_PAIR(rtw_mi_netif_stop_queue, rtw_mi_buddy_netif_stop_queue, op_stop);
MI_NETIF_PAIR(rtw_mi_netif_wake_queue, rtw_mi_buddy_netif_wake_queue, op_wake);
MI_NETIF_PAIR(rtw_mi_netif_carrier_on, rtw_mi_buddy_netif_carrier_on, op_carr_on);
MI_NETIF_PAIR(rtw_mi_netif_carrier_off, rtw_mi_buddy_netif_carrier_off, op_carr_off);
