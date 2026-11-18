// SPDX-License-Identifier: GPL-2.0
/* L1 symbol stubs for W4-119 MBSSID CAM (body in rust/hal_com_mbid_cam.rs). */

struct dvobj_priv;
struct _adapter;

void rtw_mbid_cam_init(struct dvobj_priv *dvobj) { (void)dvobj; }
void rtw_mbid_cam_deinit(struct dvobj_priv *dvobj) { (void)dvobj; }
void rtw_mbid_cam_reset(struct _adapter *adapter) { (void)adapter; }

unsigned char rtw_mbid_cam_search_by_macaddr(struct _adapter *a, unsigned char *mac)
{
	(void)a;
	(void)mac;
	return 0xFF;
}

unsigned char rtw_mbid_cam_search_by_ifaceid(struct _adapter *a, unsigned char iface_id)
{
	(void)a;
	(void)iface_id;
	return 0xFF;
}

unsigned char rtw_get_max_mbid_cam_id(struct _adapter *a)
{
	(void)a;
	return 0xFF;
}

unsigned char rtw_get_mbid_cam_entry_num(struct _adapter *a)
{
	(void)a;
	return 0;
}
