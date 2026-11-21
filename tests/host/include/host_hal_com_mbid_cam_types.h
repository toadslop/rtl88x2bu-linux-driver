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

void host_sel_reset(struct host_sel_capture *cap);
int host_sel_string(struct host_sel_capture *cap, char *out, size_t outlen);
void host_mbid_cam_set_iface_role(u8 iface_id, unsigned int fw_state);

#endif