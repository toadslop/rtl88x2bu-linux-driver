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

unsigned char rtw_mbid_camid_alloc(struct _adapter *a, unsigned char *mac)
{
	(void)a;
	(void)mac;
	return 0xFF;
}

unsigned char rtw_mbid_cam_info_change(struct _adapter *a, unsigned char *mac)
{
	(void)a;
	(void)mac;
	return 0xFF;
}

unsigned char rtw_mbid_cam_assign(struct _adapter *a, unsigned char *mac, unsigned char camid)
{
	(void)a;
	(void)mac;
	(void)camid;
	return 0;
}

void rtw_mbid_camid_clean(struct _adapter *a, unsigned char camid)
{
	(void)a;
	(void)camid;
}

int rtw_mbid_cam_cache_dump(void *sel, const char *fn, struct _adapter *a)
{
	(void)sel;
	(void)fn;
	(void)a;
	return 0;
}

void write_mbssid_cam(struct _adapter *a, unsigned char cam, unsigned char *mac)
{
	(void)a;
	(void)cam;
	(void)mac;
}

void rtw_ap_set_mbid_num(struct _adapter *a, unsigned char ap_num)
{
	(void)a;
	(void)ap_num;
}

void rtw_mbid_cam_enable(struct _adapter *a)
{
	(void)a;
}

void rtw_mi_set_mbid_cam(struct _adapter *a)
{
	(void)a;
}

void rtw_hal_set_macaddr_mbid(struct _adapter *a, unsigned char *mac)
{
	(void)a;
	(void)mac;
}

void rtw_hal_change_macaddr_mbid(struct _adapter *a, unsigned char *mac)
{
	(void)a;
	(void)mac;
}

int rtw_mbid_cam_dump(void *sel, const char *fn, struct _adapter *a)
{
	(void)sel;
	(void)fn;
	(void)a;
	return 0;
}
