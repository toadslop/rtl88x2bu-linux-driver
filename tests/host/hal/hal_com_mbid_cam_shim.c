// SPDX-License-Identifier: GPL-2.0
/* Host fixture for W4-119 MBSSID CAM L2. */

#include "host_hal_com_mbid_cam_types.h"
#include <stdint.h>
#include <string.h>

static struct dvobj_priv g_dvobj;
static struct _adapter g_adapter;
static struct _adapter g_adapters[CONFIG_IFACE_NUMBER];
static int g_sel_overflow;
static u8 g_force_cannot_run;
static u8 g_cannot_run_after_n_poll_reads;

struct host_mbid_cam_hw_trace host_mbid_cam_hw_trace;
struct host_mbid_cam_read_trace host_mbid_cam_read_trace;
struct host_mbid_cam_read_mmio host_mbid_cam_read_mmio;

void host_mbid_cam_oracle_reset_warn(void);

void host_mbid_cam_read_reset(void)
{
	memset(&host_mbid_cam_read_trace, 0, sizeof(host_mbid_cam_read_trace));
	memset(&host_mbid_cam_read_mmio, 0, sizeof(host_mbid_cam_read_mmio));
}

struct host_mbid_cam_read_trace *host_mbid_cam_read_trace_ptr(void)
{
	return &host_mbid_cam_read_trace;
}

void host_mbid_cam_set_force_cannot_run(u8 v)
{
	g_force_cannot_run = v;
}

void host_mbid_cam_set_cannot_run_after_poll_reads(u8 n)
{
	g_cannot_run_after_n_poll_reads = n;
}

void host_mbid_cam_read_seed(u8 cam_addr, const u8 *mac, u8 poll_reads_before_clear)
{
	if (cam_addr >= TOTAL_MBID_CAM_NUM)
		return;
	if (mac)
		memcpy(host_mbid_cam_read_mmio.slots[cam_addr].mac, mac, ETH_ALEN);
	host_mbid_cam_read_mmio.slots[cam_addr].poll_reads_before_clear =
		poll_reads_before_clear ? poll_reads_before_clear : 1;
	host_mbid_cam_read_mmio.slots[cam_addr].valid = 1;
}

void host_mbid_cam_read_mmio_apply_cam(u8 cam_addr)
{
	const u8 *mac;

	if (cam_addr >= TOTAL_MBID_CAM_NUM ||
	    !host_mbid_cam_read_mmio.slots[cam_addr].valid)
		return;
	mac = host_mbid_cam_read_mmio.slots[cam_addr].mac;
	host_mbid_cam_read_mmio.reg_cfg1 =
		(u32)mac[0] | ((u32)mac[1] << 8) | ((u32)mac[2] << 16) |
		((u32)mac[3] << 24);
	host_mbid_cam_read_mmio.reg_cfg2 =
		(host_mbid_cam_read_mmio.reg_cfg2 & ~0xffffu) |
		(((u32)mac[5] << 8) | (u32)mac[4]);
}

void host_mbid_cam_hw_reset(void)
{
	memset(&host_mbid_cam_hw_trace, 0, sizeof(host_mbid_cam_hw_trace));
	host_mbid_cam_hw_trace.reg_mbid_num = 0;
}

void host_mbid_cam_reset_fixture(void)
{
	unsigned int i;

	host_mbid_cam_oracle_reset_warn();
	host_mbid_cam_hw_reset();
	host_mbid_cam_read_reset();
	g_force_cannot_run = 0;
	g_cannot_run_after_n_poll_reads = 0;
	memset(&g_dvobj, 0, sizeof(g_dvobj));
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(g_adapters, 0, sizeof(g_adapters));
	g_sel_overflow = 0;
	for (i = 0; i < CONFIG_IFACE_NUMBER; i++) {
		g_adapters[i].iface_id = (u8)i;
		g_dvobj.padapters[i] = &g_adapters[i];
	}
}

void host_mbid_cam_set_iface_role(u8 iface_id, unsigned int fw_state)
{
	if (iface_id < CONFIG_IFACE_NUMBER)
		g_adapters[iface_id].fw_state = fw_state;
}

void host_sel_reset(struct host_sel_capture *cap)
{
	cap->len = 0;
	cap->buf[0] = '\0';
	g_sel_overflow = 0;
}

int host_sel_string(struct host_sel_capture *cap, char *out, size_t outlen)
{
	if (g_sel_overflow || cap->len >= outlen)
		return -1;
	memcpy(out, cap->buf, cap->len + 1);
	return (int)cap->len;
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

void *_rtw_memset(void *dst, int val, unsigned int sz)
{
	return memset(dst, val, sz);
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

u8 rtw_rust_hal_com_read8(void *adapter, unsigned int addr)
{
	(void)adapter;
	if (addr == HOST_REG_MBID_NUM)
		return host_mbid_cam_hw_trace.reg_mbid_num;
	return 0;
}

void rtw_rust_hal_com_write8(void *adapter, unsigned int addr, u8 val)
{
	(void)adapter;
	if (addr == HOST_REG_MBID_NUM)
		host_mbid_cam_hw_trace.reg_mbid_num = val;
}

int rtw_rust_hal_com_hw_var_mbssid_cam_write(void)
{
	return HOST_HW_VAR_MBSSID_CAM_WRITE;
}

void rtw_hal_set_hwreg(struct _adapter *padapter, unsigned int variable, u8 *val)
{
	unsigned idx;

	(void)padapter;
	if (variable != HOST_HW_VAR_MBSSID_CAM_WRITE || !val)
		return;
	idx = host_mbid_cam_hw_trace.hwreg_count;
	if (idx >= TOTAL_MBID_CAM_NUM)
		return;
	host_mbid_cam_hw_trace.writes[idx].cam_val[0] =
		(u32)val[0] | ((u32)val[1] << 8) | ((u32)val[2] << 16) |
		((u32)val[3] << 24);
	host_mbid_cam_hw_trace.writes[idx].cam_val[1] =
		(u32)val[4] | ((u32)val[5] << 8) | ((u32)val[6] << 16) |
		((u32)val[7] << 24);
	host_mbid_cam_hw_trace.writes[idx].cam_id =
		(u8)((host_mbid_cam_hw_trace.writes[idx].cam_val[1] >> 24) &
		     0x1f);
	host_mbid_cam_hw_trace.hwreg_count++;
}

void rtw_rust_hal_com_set_hwreg(void *adapter, unsigned int variable, u8 *val)
{
	rtw_hal_set_hwreg(adapter, variable, val);
}

u8 rtw_hal_rcr_add(void *adapter, unsigned int add)
{
	(void)adapter;
	host_mbid_cam_hw_trace.rcr_add_count++;
	host_mbid_cam_hw_trace.last_rcr_add = add;
	return 0;
}

void rtw_rust_hal_com_mbid_log_mi_set_cam(const char *func, u8 cam_id, u8 *mac)
{
	(void)func;
	(void)cam_id;
	(void)mac;
}

void rtw_rust_hal_com_mbid_log_set_macaddr(const char *func, void *adapter,
					   u8 entry_id, u8 *mac)
{
	(void)func;
	(void)adapter;
	(void)entry_id;
	(void)mac;
}

void *rtw_rust_hal_com_dvobj_padapter(void *dvobj, u8 iface_id)
{
	if (!dvobj || iface_id >= CONFIG_IFACE_NUMBER)
		return NULL;
	return ((struct dvobj_priv *)dvobj)->padapters[iface_id];
}

static int host_check_fw(uint32_t fw, uint32_t state)
{
	return (fw & state) == state;
}

u8 rtw_rust_hal_com_mlme_is_sta(void *adapter)
{
	if (!adapter)
		return 0;
	return host_check_fw(((struct _adapter *)adapter)->fw_state, WIFI_STATION_STATE);
}

u8 rtw_rust_hal_com_mlme_is_ap(void *adapter)
{
	if (!adapter)
		return 0;
	return host_check_fw(((struct _adapter *)adapter)->fw_state, WIFI_AP_STATE);
}

u8 rtw_rust_hal_com_mlme_is_mesh(void *adapter)
{
	if (!adapter)
		return 0;
	return host_check_fw(((struct _adapter *)adapter)->fw_state, WIFI_MESH_STATE);
}

void rtw_rust_hal_com_print_line(void *sel, const char *line)
{
	struct host_sel_capture *cap = sel;
	size_t n;

	if (!cap || !line)
		return;
	n = strlen(line);
	if (cap->len + n >= sizeof(cap->buf)) {
		g_sel_overflow = 1;
		return;
	}
	memcpy(cap->buf + cap->len, line, n);
	cap->len += n;
	cap->buf[cap->len] = '\0';
}

u8 rtw_rust_hal_com_cannot_run(void *adapter)
{
	(void)adapter;
	return g_force_cannot_run;
}

u32 rtw_rust_hal_com_read32(void *adapter, u32 addr)
{
	(void)adapter;
	if (addr == HOST_REG_MBIDCAMCFG_2) {
		host_mbid_cam_read_trace.cfg2_read32_count++;
		if (host_mbid_cam_read_mmio.reg_cfg2 & HOST_BIT_MBIDCAM_POLL) {
			host_mbid_cam_read_mmio.poll_read_count++;
			if (g_cannot_run_after_n_poll_reads &&
			    host_mbid_cam_read_mmio.poll_read_count >=
			    g_cannot_run_after_n_poll_reads)
				g_force_cannot_run = 1;
			if (host_mbid_cam_read_mmio.poll_reads_left) {
				host_mbid_cam_read_mmio.poll_reads_left--;
				if (!host_mbid_cam_read_mmio.poll_reads_left) {
					host_mbid_cam_read_mmio.reg_cfg2 &=
						~HOST_BIT_MBIDCAM_POLL;
					host_mbid_cam_read_mmio_apply_cam(
						host_mbid_cam_read_mmio.active_cam);
				}
			}
		}
		return host_mbid_cam_read_mmio.reg_cfg2;
	}
	if (addr == HOST_REG_MBIDCAMCFG_1) {
		host_mbid_cam_read_trace.cfg1_read32_count++;
		return host_mbid_cam_read_mmio.reg_cfg1;
	}
	return 0;
}

u16 rtw_rust_hal_com_read16(void *adapter, u32 addr)
{
	(void)adapter;
	if (addr == HOST_REG_MBIDCAMCFG_2) {
		host_mbid_cam_read_trace.cfg2_read16_count++;
		return (u16)(host_mbid_cam_read_mmio.reg_cfg2 & 0xffffu);
	}
	return 0;
}

void rtw_rust_hal_com_write32(void *adapter, u32 addr, u32 val)
{
	u8 cam_addr;

	(void)adapter;
	if (addr != HOST_REG_MBIDCAMCFG_2)
		return;
	host_mbid_cam_read_trace.cfg2_write_count++;
	host_mbid_cam_read_trace.last_cfg2_write = val;
	if (!(val & HOST_BIT_MBIDCAM_POLL))
		return;
	cam_addr = (u8)((val >> HOST_MBIDCAM_ADDR_SHIFT) & HOST_MBIDCAM_ADDR_MASK);
	host_mbid_cam_read_trace.last_cam_addr = cam_addr;
	host_mbid_cam_read_mmio.active_cam = cam_addr;
	host_mbid_cam_read_mmio.poll_read_count = 0;
	host_mbid_cam_read_mmio.reg_cfg1 = 0;
	if (cam_addr < TOTAL_MBID_CAM_NUM &&
	    host_mbid_cam_read_mmio.slots[cam_addr].valid)
		host_mbid_cam_read_mmio.poll_reads_left =
			host_mbid_cam_read_mmio.slots[cam_addr].poll_reads_before_clear;
	else
		host_mbid_cam_read_mmio.poll_reads_left = 1;
	host_mbid_cam_read_mmio.reg_cfg2 = val;
}