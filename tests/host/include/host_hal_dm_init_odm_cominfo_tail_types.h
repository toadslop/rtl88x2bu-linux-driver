// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_INIT_ODM_COMINFO_TAIL_TYPES_H
#define HOST_HAL_DM_INIT_ODM_COMINFO_TAIL_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

struct dm_struct {
	void *adapter;
};

struct hal_data {
	u8 current_channel;
	u8 b_scan_in_process;
	u8 antenna_test;
	u8 multi_ch_switch_mode;
	u8 phydm_op_mode;
	struct dm_struct odmpriv;
};

struct registry_priv {
	u32 halrf_ability;
};

struct rtw_traffic_statistics {
	u32 cur_tx_tp;
	u32 cur_rx_tp;
};

struct dvobj_priv {
	struct rtw_traffic_statistics traffic_stat;
	u8 usb_speed;
};

struct pwrctrl_priv {
	u8 bpower_saving;
};

struct _adapter {
	struct hal_data hal;
	struct registry_priv registrypriv;
	struct dvobj_priv dvobj;
};

#define GET_HAL_DATA(adapter) (&(adapter)->hal)

struct host_hal_dm_init_odm_tail_trace {
	void *chnl;
	void *scan;
	void *ps;
	void *usb_speed;
	int sta_hooks;
	int finish;
};

extern struct host_hal_dm_init_odm_tail_trace host_hal_dm_init_odm_tail_trace;

void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr);
void halrf_cmn_info_hook(struct dm_struct *dm, u8 id, void *ptr);

void rtw_rust_hal_dm_init_odm_cominfo_tail(struct _adapter *adapter);

#endif