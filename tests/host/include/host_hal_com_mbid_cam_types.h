/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_MBID_CAM_TYPES_H
#define HOST_HAL_COM_MBID_CAM_TYPES_H

#include <stddef.h>
#include <string.h>

#define ETH_ALEN 6
#define TOTAL_MBID_CAM_NUM 8
#define INVALID_CAM_ID 0xFF
#define CONFIG_IFACE_NUMBER 4
#define WIFI_STATION_STATE 0x00000008u
#define WIFI_AP_STATE 0x00000010u
#define WIFI_MESH_STATE 0x00000200u
#define BIT(x) (1U << (x))
#define _TRUE 1
#define _FALSE 0

#define ATOMIC_INC(v) (*(v) += 1)
#define ATOMIC_DEC(v) (*(v) -= 1)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int ATOMIC_T, _lock;

#define ATOMIC_SET(v, x) (*(v) = (x))
#define ATOMIC_READ(v) (*(v))

struct mbid_cam_cache {
	u8 iface_id;
	u8 mac_addr[ETH_ALEN];
};

struct mbid_cam_ctl_t {
	_lock lock;
	u8 bitmap;
	ATOMIC_T mbid_entry_num;
};

struct host_sel_capture {
	size_t len;
	char buf[4096];
};

struct dvobj_priv {
	struct mbid_cam_ctl_t mbid_cam_ctl;
	struct mbid_cam_cache mbid_cam_cache[TOTAL_MBID_CAM_NUM];
	struct _adapter *padapters[CONFIG_IFACE_NUMBER];
};

struct _adapter {
	u8 iface_id;
	unsigned int fw_state;
};

#define HOST_REG_MBID_NUM 0x0552u
#define HOST_REG_MBIDCAMCFG_1 0x0628u
#define HOST_REG_MBIDCAMCFG_2 0x062cu
#define HOST_BIT_MBIDCAM_POLL (1u << 31)
#define HOST_MBIDCAM_ADDR_MASK 0x1fu
#define HOST_MBIDCAM_ADDR_SHIFT 24u
#define HOST_HW_VAR_MBSSID_CAM_WRITE 100
#define HOST_RCR_ENMBID (1u << 24)

struct host_mbid_cam_hw_write {
	u8 cam_id;
	u32 cam_val[2];
};

struct host_mbid_cam_hw_trace {
	u8 reg_mbid_num;
	unsigned hwreg_count;
	unsigned rcr_add_count;
	u32 last_rcr_add;
	struct host_mbid_cam_hw_write writes[TOTAL_MBID_CAM_NUM];
};

extern struct host_mbid_cam_hw_trace host_mbid_cam_hw_trace;

struct host_mbid_cam_read_trace {
	unsigned cfg2_write_count;
	unsigned cfg2_read32_count;
	unsigned cfg1_read32_count;
	unsigned cfg2_read16_count;
	u32 last_cfg2_write;
	u8 last_cam_addr;
	u8 mac_out[ETH_ALEN];
};

struct host_mbid_cam_read_slot_seed {
	u8 mac[ETH_ALEN];
	u8 poll_reads_before_clear;
	u8 valid;
};

struct host_mbid_cam_read_mmio {
	u32 reg_cfg1;
	u32 reg_cfg2;
	u8 poll_reads_left;
	u8 active_cam;
	u8 poll_read_count;
	struct host_mbid_cam_read_slot_seed slots[TOTAL_MBID_CAM_NUM];
};

extern struct host_mbid_cam_read_trace host_mbid_cam_read_trace;
extern struct host_mbid_cam_read_mmio host_mbid_cam_read_mmio;

void host_mbid_cam_read_reset(void);
void host_mbid_cam_read_seed(u8 cam_addr, const u8 *mac, u8 poll_reads_before_clear);
struct host_mbid_cam_read_trace *host_mbid_cam_read_trace_ptr(void);
void host_mbid_cam_read_mmio_apply_cam(u8 cam_addr);
void host_mbid_cam_set_force_cannot_run(u8 v);
void host_mbid_cam_set_cannot_run_after_poll_reads(u8 n);

void host_mbid_cam_hw_reset(void);
void host_sel_reset(struct host_sel_capture *cap);
int host_sel_string(struct host_sel_capture *cap, char *out, size_t outlen);
void host_mbid_cam_set_iface_role(u8 iface_id, unsigned int fw_state);

#endif