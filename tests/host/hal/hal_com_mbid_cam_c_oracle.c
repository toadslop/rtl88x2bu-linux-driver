// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_mbid_cam_types.h"
#include <stdarg.h>
#include <stdio.h>

struct dvobj_priv *_adapter_to_dvobj(struct _adapter *adapter);

static unsigned oracle_warn_count;

unsigned host_mbid_cam_oracle_warn_count(void);
void host_mbid_cam_oracle_reset_warn(void);

unsigned host_mbid_cam_oracle_warn_count(void)
{
	return oracle_warn_count;
}

void host_mbid_cam_oracle_reset_warn(void)
{
	oracle_warn_count = 0;
}

void host_mbid_cam_record_warn(void)
{
	oracle_warn_count++;
}

static void oracle_warn(void)
{
	host_mbid_cam_record_warn();
}

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

static void cache_init(struct _adapter *a, struct mbid_cam_cache *ent, u8 *mac)
{
	if (a && ent && mac) {
		memcpy(ent->mac_addr, mac, ETH_ALEN);
		ent->iface_id = a->iface_id;
	}
}

static void cache_clr(struct mbid_cam_cache *ent)
{
	if (ent) {
		memset(ent->mac_addr, 0, ETH_ALEN);
		ent->iface_id = CONFIG_IFACE_NUMBER;
	}
}

u8 rtw_mbid_camid_alloc_c_oracle(struct _adapter *adapter, u8 *mac_addr)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	struct mbid_cam_ctl_t *ctl = &dvobj->mbid_cam_ctl;
	u8 cam_id = INVALID_CAM_ID;
	u8 entry_num = (u8)ATOMIC_READ(&ctl->mbid_entry_num);
	u8 i;

	if (search_mac(adapter, mac_addr) != INVALID_CAM_ID)
		return INVALID_CAM_ID;

	if (entry_num >= TOTAL_MBID_CAM_NUM)
		oracle_warn();

	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		if (!(ctl->bitmap & BIT(i))) {
			ctl->bitmap |= BIT(i);
			cam_id = i;
			break;
		}
	}
	if (cam_id != INVALID_CAM_ID && mac_addr)
		cache_init(adapter, &dvobj->mbid_cam_cache[cam_id], mac_addr);

	if (cam_id != INVALID_CAM_ID)
		ATOMIC_INC(&ctl->mbid_entry_num);
	return cam_id;
}

u8 rtw_mbid_cam_info_change_c_oracle(struct _adapter *adapter, u8 *mac_addr)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	u8 entry_id = search_iface(adapter, adapter->iface_id);

	if (entry_id != INVALID_CAM_ID)
		cache_init(adapter, &dvobj->mbid_cam_cache[entry_id], mac_addr);
	return entry_id;
}

u8 rtw_mbid_cam_assign_c_oracle(struct _adapter *adapter, u8 *mac_addr, u8 camid)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	struct mbid_cam_ctl_t *ctl = &dvobj->mbid_cam_ctl;
	u8 ret = _FALSE;

	if (camid >= TOTAL_MBID_CAM_NUM || camid == INVALID_CAM_ID)
		oracle_warn();
	if (search_mac(adapter, mac_addr) != INVALID_CAM_ID)
		return _FALSE;

	if (!(ctl->bitmap & BIT(camid)) && mac_addr) {
		ctl->bitmap |= BIT(camid);
		/* Driver indexes cache even for invalid camid; guard on host. */
		if (camid < TOTAL_MBID_CAM_NUM)
			cache_init(adapter, &dvobj->mbid_cam_cache[camid], mac_addr);
		ret = _TRUE;
	}

	if (ret == _TRUE)
		ATOMIC_INC(&ctl->mbid_entry_num);
	return ret;
}

static int mlme_is_sta(struct _adapter *iface)
{
	return (iface->fw_state & WIFI_STATION_STATE) == WIFI_STATION_STATE;
}

static int mlme_is_ap(struct _adapter *iface)
{
	return (iface->fw_state & WIFI_AP_STATE) == WIFI_AP_STATE;
}

static int mlme_is_mesh(struct _adapter *iface)
{
	return (iface->fw_state & WIFI_MESH_STATE) == WIFI_MESH_STATE;
}

static void oracle_print_line(void *sel, const char *line)
{
	struct host_sel_capture *cap = sel;
	size_t n;

	if (!cap || !line)
		return;
	n = strlen(line);
	if (cap->len + n >= sizeof(cap->buf))
		return;
	memcpy(cap->buf + cap->len, line, n);
	cap->len += n;
	cap->buf[cap->len] = '\0';
}

static void oracle_printf(void *sel, const char *fmt, ...)
{
	char line[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(line, sizeof(line), fmt, ap);
	va_end(ap);
	oracle_print_line(sel, line);
}

void rtw_mbid_cam_cache_dump_c_oracle(void *sel, const char *fun_name,
				      struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	struct mbid_cam_ctl_t *ctl = &dvobj->mbid_cam_ctl;
	u8 entry_num = (u8)ATOMIC_READ(&ctl->mbid_entry_num);
	u8 max_cam_id = rtw_get_max_mbid_cam_id_c_oracle(adapter);
	u8 i;

	oracle_printf(sel, "== MBSSID CAM DUMP (%s)==\n", fun_name);
	oracle_printf(sel, "Entry numbers:%u, max_camid:%u, bitmap:0x%08x\n",
		      entry_num, max_cam_id, ctl->bitmap);
	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		oracle_printf(sel, "CAM_ID = %u\t", i);
		if (ctl->bitmap & BIT(i)) {
			u8 iface_id = dvobj->mbid_cam_cache[i].iface_id;
			struct _adapter *iface = dvobj->padapters[iface_id];

			oracle_printf(sel, "IF_ID:%u\t", iface_id);
			oracle_printf(sel,
				      "MAC Addr:%02x:%02x:%02x:%02x:%02x:%02x\t",
				      dvobj->mbid_cam_cache[i].mac_addr[0],
				      dvobj->mbid_cam_cache[i].mac_addr[1],
				      dvobj->mbid_cam_cache[i].mac_addr[2],
				      dvobj->mbid_cam_cache[i].mac_addr[3],
				      dvobj->mbid_cam_cache[i].mac_addr[4],
				      dvobj->mbid_cam_cache[i].mac_addr[5]);
			if (iface) {
				if (mlme_is_sta(iface))
					oracle_printf(sel, "ROLE:%s\n", "STA");
				else if (mlme_is_ap(iface))
					oracle_printf(sel, "ROLE:%s\n", "AP");
				else if (mlme_is_mesh(iface))
					oracle_printf(sel, "ROLE:%s\n", "MESH");
				else
					oracle_printf(sel, "ROLE:%s\n", "NONE");
			}
		} else {
			oracle_printf(sel, "N/A\n");
		}
	}
}

void rtw_mbid_camid_clean_c_oracle(struct _adapter *adapter, u8 mbss_camid)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	struct mbid_cam_ctl_t *ctl = &dvobj->mbid_cam_ctl;

	if (mbss_camid >= TOTAL_MBID_CAM_NUM || mbss_camid == INVALID_CAM_ID)
		oracle_warn();
	/* Driver clears cache even for invalid camid; guard on host. */
	if (mbss_camid < TOTAL_MBID_CAM_NUM)
		cache_clr(&dvobj->mbid_cam_cache[mbss_camid]);
	ctl->bitmap &= (u8)(~BIT(mbss_camid));
	ATOMIC_DEC(&ctl->mbid_entry_num);
}

#define MBIDCAM_ADDR_MASK 0x1Fu
#define MBIDCAM_ADDR_SHIFT 24u

static u8 oracle_reg_mbid_num;
static struct host_mbid_cam_hw_trace oracle_hw;

void host_mbid_cam_hw_oracle_reset(void)
{
	oracle_reg_mbid_num = 0;
	memset(&oracle_hw, 0, sizeof(oracle_hw));
}

void host_mbid_cam_hw_oracle_set_reg(u8 val)
{
	oracle_reg_mbid_num = val;
}

static void oracle_record_hwreg(u8 cam_addr, u8 *mac)
{
	unsigned idx = oracle_hw.hwreg_count;
	u32 cam_val[2] = { 0 };
	unsigned i;

	if (idx >= TOTAL_MBID_CAM_NUM || !mac)
		return;
	cam_val[0] = ((u32)mac[3] << 24) | ((u32)mac[2] << 16) |
		     ((u32)mac[1] << 8) | mac[0];
	cam_val[1] = ((cam_addr & MBIDCAM_ADDR_MASK) << MBIDCAM_ADDR_SHIFT) |
		     ((u32)mac[5] << 8) | mac[4];
	oracle_hw.writes[idx].cam_val[0] = cam_val[0];
	oracle_hw.writes[idx].cam_val[1] = cam_val[1];
	oracle_hw.writes[idx].cam_id = cam_addr;
	oracle_hw.hwreg_count++;
	(void)i;
}

void rtw_ap_set_mbid_num_c_oracle(struct _adapter *adapter, u8 ap_num)
{
	(void)adapter;
	oracle_reg_mbid_num =
		(oracle_reg_mbid_num & 0xF8) | ((ap_num - 1) & 0x07);
}

void rtw_mi_set_mbid_cam_c_oracle(struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = _adapter_to_dvobj(adapter);
	struct mbid_cam_ctl_t *ctl = &dvobj->mbid_cam_ctl;
	u8 i;

	for (i = 0; i < TOTAL_MBID_CAM_NUM; i++) {
		if (ctl->bitmap & BIT(i))
			oracle_record_hwreg(i, dvobj->mbid_cam_cache[i].mac_addr);
	}
	oracle_hw.rcr_add_count++;
	oracle_hw.last_rcr_add = HOST_RCR_ENMBID;
}

struct host_mbid_cam_hw_trace *host_mbid_cam_hw_oracle_trace(void)
{
	oracle_hw.reg_mbid_num = oracle_reg_mbid_num;
	return &oracle_hw;
}