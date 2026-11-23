// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-119 MBSSID CAM L2. */

#include "host_hal_com_mbid_cam_types.h"
#include <string.h>

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;

void host_mbid_cam_oracle_reset_warn(void);

void host_mbid_cam_reset_fixture(void)
{
	host_mbid_cam_oracle_reset_warn();
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

void *rtw_rust_hal_com_adapter_to_dvobj(void *adapter)
{
	return _adapter_to_dvobj(adapter);
}

void *rtw_rust_hal_com_mbid_cam_ctl(void *dvobj)
{
	return &((struct dvobj_priv *)dvobj)->mbid_cam_ctl;
}

void rtw_rust_hal_com_spinlock_init(void *lock)
{
	(void)lock;
}

void rtw_rust_hal_com_spinlock_free(void *lock)
{
	(void)lock;
}

void rtw_rust_hal_com_enter_critical_bh(void *lock, void *irqL)
{
	(void)lock;
	(void)irqL;
}

void rtw_rust_hal_com_exit_critical_bh(void *lock, void *irqL)
{
	(void)lock;
	(void)irqL;
}

void *rtw_rust_hal_com_mbid_ctl_lock(void *ctl)
{
	return &((struct mbid_cam_ctl_t *)ctl)->lock;
}

u8 rtw_rust_hal_com_mbid_ctl_bitmap(void *ctl)
{
	return ((struct mbid_cam_ctl_t *)ctl)->bitmap;
}

void rtw_rust_hal_com_mbid_ctl_set_bitmap(void *ctl, u8 bitmap)
{
	((struct mbid_cam_ctl_t *)ctl)->bitmap = bitmap;
}

void *rtw_rust_hal_com_mbid_ctl_entry_num(void *ctl)
{
	return &((struct mbid_cam_ctl_t *)ctl)->mbid_entry_num;
}

int rtw_rust_hal_com_atomic_read(void *v)
{
	return ATOMIC_READ((ATOMIC_T *)v);
}

void rtw_rust_hal_com_atomic_set(void *v, int x)
{
	ATOMIC_SET((ATOMIC_T *)v, x);
}

void rtw_rust_hal_com_mbid_cam_cache_zero(void *dvobj)
{
	memset(((struct dvobj_priv *)dvobj)->mbid_cam_cache, 0,
	       sizeof(((struct dvobj_priv *)dvobj)->mbid_cam_cache));
}

u8 rtw_rust_hal_com_mbid_cache_iface_id(void *dvobj, u8 idx)
{
	if (idx >= TOTAL_MBID_CAM_NUM)
		return CONFIG_IFACE_NUMBER;
	return ((struct dvobj_priv *)dvobj)->mbid_cam_cache[idx].iface_id;
}

u8 *rtw_rust_hal_com_mbid_cache_mac(void *dvobj, u8 idx)
{
	if (idx >= TOTAL_MBID_CAM_NUM)
		return NULL;
	return ((struct dvobj_priv *)dvobj)->mbid_cam_cache[idx].mac_addr;
}

void rtw_rust_hal_com_mbid_log_mac_search(const char *func, u8 *mac_addr, u8 cam_id)
{
	(void)func;
	(void)mac_addr;
	(void)cam_id;
}

void rtw_rust_hal_com_mbid_log_iface_search(const char *func, u8 iface_id,
					    const u8 *mac_addr, u8 cam_id)
{
	(void)func;
	(void)iface_id;
	(void)mac_addr;
	(void)cam_id;
}

void rtw_rust_hal_com_mbid_log_alloc_over_limit(void *adapter, u8 entry_num)
{
	(void)adapter;
	(void)entry_num;
}

void rtw_rust_hal_com_mbid_log_alloc_result(const char *func, u8 *mac_addr, u8 cam_id)
{
	(void)func;
	(void)mac_addr;
	(void)cam_id;
}

void rtw_rust_hal_com_mbid_log_invalid_camid(void *adapter, u8 camid)
{
	(void)adapter;
	(void)camid;
}

void rtw_rust_hal_com_mbid_log_assign_result(const char *func, u8 *mac_addr,
					     u8 camid, u8 success)
{
	(void)func;
	(void)mac_addr;
	(void)camid;
	(void)success;
}

void rtw_rust_hal_com_mbid_log_clean(const char *func, u8 camid)
{
	(void)func;
	(void)camid;
}

int _rtw_memcmp(const void *dst, const void *src, unsigned int sz)
{
	return memcmp(dst, src, sz) == 0 ? 1 : 0;
}

u8 rtw_rust_hal_com_adapter_iface_id(void *adapter)
{
	return ((struct _adapter *)adapter)->iface_id;
}

u8 rtw_rust_hal_com_mbid_ctl_bitmap_test(void *ctl, u8 idx)
{
	return (((struct mbid_cam_ctl_t *)ctl)->bitmap & BIT(idx)) != 0;
}

void rtw_rust_hal_com_mbid_ctl_bitmap_set_bit(void *ctl, u8 idx)
{
	((struct mbid_cam_ctl_t *)ctl)->bitmap |= BIT(idx);
}

void rtw_rust_hal_com_mbid_ctl_bitmap_clr_bit(void *ctl, u8 idx)
{
	((struct mbid_cam_ctl_t *)ctl)->bitmap &= ~BIT(idx);
}

void rtw_rust_hal_com_atomic_inc(ATOMIC_T *v)
{
	ATOMIC_INC(v);
}

void rtw_rust_hal_com_atomic_dec(ATOMIC_T *v)
{
	ATOMIC_DEC(v);
}

void rtw_rust_hal_com_mbid_cache_init(void *adapter, void *dvobj, u8 idx, u8 *mac)
{
	struct _adapter *a = adapter;
	struct dvobj_priv *d = dvobj;

	if (!a || !d || !mac || idx >= TOTAL_MBID_CAM_NUM)
		return;
	memcpy(d->mbid_cam_cache[idx].mac_addr, mac, ETH_ALEN);
	d->mbid_cam_cache[idx].iface_id = a->iface_id;
}

void rtw_rust_hal_com_mbid_cache_clr(void *dvobj, u8 idx)
{
	struct dvobj_priv *d = dvobj;

	if (!d || idx >= TOTAL_MBID_CAM_NUM)
		return;
	memset(d->mbid_cam_cache[idx].mac_addr, 0, ETH_ALEN);
	d->mbid_cam_cache[idx].iface_id = CONFIG_IFACE_NUMBER;
}

void host_mbid_cam_record_warn(void);

void rtw_rust_hal_com_warn_on(int condition)
{
	if (condition)
		host_mbid_cam_record_warn();
}

/* Stubs so test-mbid-cam-rust links after PR1 adds rtw_mbid_cam_cache_dump (L2 in PR2). */
void rtw_rust_hal_com_print_line(void *sel, const char *line)
{
	(void)sel;
	(void)line;
}

void *rtw_rust_hal_com_dvobj_padapter(void *dvobj, u8 iface_id)
{
	(void)dvobj;
	(void)iface_id;
	return NULL;
}

u8 rtw_rust_hal_com_mlme_is_sta(void *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_hal_com_mlme_is_ap(void *adapter)
{
	(void)adapter;
	return 0;
}

u8 rtw_rust_hal_com_mlme_is_mesh(void *adapter)
{
	(void)adapter;
	return 0;
}