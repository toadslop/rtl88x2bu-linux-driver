/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_TSF_PERIODIC_TYPES_H
#define HOST_HAL_COM_TSF_PERIODIC_TYPES_H

#include "host_hal_com_tsf_types.h"

#define OFFCHS_NONE 0
#define MLME_ACTION_NONE 1

struct rf_ctl_t {
	u8 offch_state;
};

struct _timer {
	int unused;
};

struct dvobj_priv {
	systime periodic_tsf_update_etime;
	struct _timer periodic_tsf_update_end_timer;
	int iface_nums;
	struct _adapter *padapters[4];
	struct rf_ctl_t rf_ctl;
	u8 surprise_removed;
	u8 drv_stopped;
};

struct host_hal_com_tsf_periodic_trace {
	unsigned rcr_chk_bssid_count;
	u8 last_rcr_chk_action;
	unsigned set_timer_count;
	u32 last_timer_ms;
	unsigned periodic_end_cmd_count;
	struct _adapter *last_end_cmd_adapter;
};

extern struct host_hal_com_tsf_periodic_trace host_hal_com_tsf_periodic_trace;

void host_hal_com_tsf_periodic_reset(void);
struct _adapter *host_hal_com_tsf_periodic_adapter(void);
struct dvobj_priv *host_hal_com_tsf_periodic_dvobj(void);
void host_hal_com_tsf_periodic_set_current_time(systime t);

void rtw_hal_periodic_tsf_update_chk(struct _adapter *adapter);
void rtw_hal_periodic_tsf_update_end_timer_hdl(void *ctx);

#endif