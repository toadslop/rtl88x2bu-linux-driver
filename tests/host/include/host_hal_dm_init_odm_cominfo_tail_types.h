// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_INIT_ODM_COMINFO_TAIL_TYPES_H
#define HOST_HAL_DM_INIT_ODM_COMINFO_TAIL_TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;

/* Reduced from kernel ODM_ASSOCIATE_ENTRY_NUM — enough to prove loop fidelity. */
#define HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM 4

#define HOST_TAIL_EVT_ODM_HOOK 1
#define HOST_TAIL_EVT_HALRF_HOOK 2
#define HOST_TAIL_EVT_STA_HOOK 3
#define HOST_TAIL_EVT_OPS_INIT 4
#define HOST_TAIL_EVT_EARLY_INIT 5

#define HOST_TAIL_MAX_EVENTS 64

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
	struct pwrctrl_priv pwrctl;
};

#define GET_HAL_DATA(adapter) (&(adapter)->hal)

static inline struct pwrctrl_priv *adapter_to_pwrctl(struct _adapter *adapter)
{
	return adapter ? &adapter->pwrctl : NULL;
}

struct host_tail_event {
	u8 kind;
	u32 id;
	void *ptr;
};

struct host_hal_dm_init_odm_tail_trace {
	struct host_tail_event events[HOST_TAIL_MAX_EVENTS];
	int count;
};

extern struct host_hal_dm_init_odm_tail_trace host_hal_dm_init_odm_tail_trace;

void host_hal_dm_init_odm_tail_reset(void);
int host_hal_dm_init_odm_tail_check_expected(struct _adapter *adapter);

void odm_cmn_info_hook(struct dm_struct *dm, u32 cmn_info, void *ptr);
void halrf_cmn_info_hook(struct dm_struct *dm, u8 id, void *ptr);
void phydm_cmn_sta_info_hook(struct dm_struct *dm, u8 macid, void *sta);
void rtw_phydm_ops_func_init(struct dm_struct *dm);
void phydm_dm_early_init(struct dm_struct *dm);

void rtw_rust_hal_dm_init_odm_cominfo_tail(struct _adapter *adapter);

/*
 * Host L2 models the default 8822B USB path only (see hal/hal_dm.c tail).
 * Not exercised here (stubbed / absent in oracle): CONFIG_RTL8723B antenna
 * path hooks, CONFIG_DYNAMIC_SOML, CONFIG_RTW_PATH_DIV, CONFIG_MP_INCLUDED
 * halrf MP hooks — same gap as the W4-88 Rust slice scope.
 */

#endif