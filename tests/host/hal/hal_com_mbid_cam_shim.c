// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-119 MBSSID CAM L2. */

#include "host_hal_com_mbid_cam_types.h"
#include <string.h>

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;

void host_mbid_cam_reset_fixture(void)
{
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
}

struct _adapter *host_mbid_cam_adapter(void)
{
	return &g_adapter;
}

struct dvobj_priv *host_mbid_cam_dvobj(void)
{
	return &g_dvobj;
}

void host_mbid_cam_prime_entry(u8 slot, u8 iface_id, const u8 *mac, u8 set_bitmap)
{
	if (slot >= TOTAL_MBID_CAM_NUM)
		return;
	g_dvobj.mbid_cam_cache[slot].iface_id = iface_id;
	if (mac)
		memcpy(g_dvobj.mbid_cam_cache[slot].mac_addr, mac, ETH_ALEN);
	if (set_bitmap)
		g_dvobj.mbid_cam_ctl.bitmap |= BIT(slot);
}

void host_mbid_cam_set_entry_num(u8 n)
{
	ATOMIC_SET(&g_dvobj.mbid_cam_ctl.mbid_entry_num, n);
}

struct dvobj_priv *_adapter_to_dvobj(struct _adapter *adapter)
{
	(void)adapter;
	return &g_dvobj;
}