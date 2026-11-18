// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_mbid_cam_types.h"

struct dvobj_priv *_adapter_to_dvobj(struct _adapter *adapter);

static u8 search_mac(struct _adapter *a, u8 *mac)
{
	struct dvobj_priv *d = _adapter_to_dvobj(a);
	u8 i;

	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++)
		if (mac && !memcmp(d->mbid_cam_cache[i].mac_addr, mac, ETH_ALEN))
			return i;
	return INVALID_CAM_ID;
}

static u8 search_iface(struct _adapter *a, u8 iface_id)
{
	struct dvobj_priv *d = _adapter_to_dvobj(a);
	u8 i;

	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++)
		if (iface_id == d->mbid_cam_cache[i].iface_id)
			return i;
	return INVALID_CAM_ID;
}

void rtw_mbid_cam_init_c_oracle(struct dvobj_priv *dvobj)
{
	dvobj->mbid_cam_ctl.bitmap = 0;
	ATOMIC_SET(&dvobj->mbid_cam_ctl.mbid_entry_num, 0);
	memset(dvobj->mbid_cam_cache, 0, sizeof(dvobj->mbid_cam_cache));
}

void rtw_mbid_cam_reset_c_oracle(struct _adapter *adapter)
{
	struct dvobj_priv *d = _adapter_to_dvobj(adapter);

	d->mbid_cam_ctl.bitmap = 0;
	memset(d->mbid_cam_cache, 0, sizeof(d->mbid_cam_cache));
	ATOMIC_SET(&d->mbid_cam_ctl.mbid_entry_num, 0);
}

u8 rtw_mbid_cam_search_by_macaddr_c_oracle(struct _adapter *a, u8 *mac)
{
	return search_mac(a, mac);
}

u8 rtw_mbid_cam_search_by_ifaceid_c_oracle(struct _adapter *a, u8 iface_id)
{
	return search_iface(a, iface_id);
}

u8 rtw_get_max_mbid_cam_id_c_oracle(struct _adapter *a)
{
	int i;
	u8 bitmap = _adapter_to_dvobj(a)->mbid_cam_ctl.bitmap;

	for (i = TOTAL_MBID_CAM_NUM - 1; i >= 0; i--)
		if (bitmap & BIT(i))
			return (u8)i;
	return INVALID_CAM_ID;
}

u8 rtw_get_mbid_cam_entry_num_c_oracle(struct _adapter *a)
{
	return (u8)ATOMIC_READ(&(_adapter_to_dvobj(a)->mbid_cam_ctl.mbid_entry_num));
}