/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_PHYDM_OPS_TYPES_H
#define HOST_HAL_DM_PHYDM_OPS_TYPES_H

#include "host_types.h"

/* Minimal PHYDM RA slice for W4-89 host oracle (matches kernel call pattern). */

enum channel_width {
	CHANNEL_WIDTH_20 = 0,
	CHANNEL_WIDTH_40 = 1,
	CHANNEL_WIDTH_80 = 2,
};

struct ra_sta_info {
	u8 rate_id;
	u8 rssi_level;
	u8 is_first_connect : 1;
	u8 is_support_sgi : 1;
	u8 is_vht_enable : 2;
	u8 disable_ra : 1;
	u8 disable_pt : 1;
	u8 txrx_state : 2;
	u8 is_noisy : 1;
	u8 curr_tx_rate;
	enum channel_width ra_bw_mode;
	enum channel_width curr_tx_bw;
	u8 curr_retry_ratio;
	u64 ramask;
};

struct cmn_sta_info {
	u16 dm_ctrl;
	enum channel_width bw_mode;
	u8 mac_id;
	u8 mac_addr[6];
	struct ra_sta_info ra_info;
};

#define HOST_MACID_CTL_NUM 4

struct macid_ctl_t {
	u8 num;
	u8 bw[HOST_MACID_CTL_NUM];
	u8 vht_en[HOST_MACID_CTL_NUM];
	u32 rate_bmp0[HOST_MACID_CTL_NUM];
	u32 rate_bmp1[HOST_MACID_CTL_NUM];
};

struct dvobj_priv {
	struct macid_ctl_t macid_ctl;
};

typedef struct {
	struct dvobj_priv dvobj;
} host_adapter;

struct host_ra_table {
	void (*record_ra_info)(void *dm_void, u8 macid, struct cmn_sta_info *p_sta,
			       u64 ra_mask);
};

struct dm_struct {
	host_adapter *adapter;
	struct host_ra_table dm_ra_table;
	void (*fill_desc_dyntxpwr)(void *dm, u8 *desc, u8 dpt_lv);
};

struct host_hal_dm_phydm_ops_trace {
	u8 init_called;
	u8 record_ptr_set;
	u8 fill_desc_ptr_set;
	u8 record_calls;
	u8 last_macid;
	u8 last_bw;
	u8 last_vht_en;
	u32 last_bmp0;
	u32 last_bmp1;
	u8 update_tx_rate_bmp_calls;
};

extern struct host_hal_dm_phydm_ops_trace host_hal_dm_phydm_ops_trace;

void host_hal_dm_phydm_ops_reset(void);

/* Declarations implemented in shim / oracle / Rust */
void rtw_phydm_ops_func_init(struct dm_struct *p_phydm);
void record_ra_info(void *p_dm_void, u8 macid, struct cmn_sta_info *p_sta,
		    u64 ra_mask);

struct dvobj_priv *adapter_to_dvobj(host_adapter *adapter);
struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *dvobj);
void rtw_macid_ctl_set_bw(struct macid_ctl_t *macid_ctl, u8 id, u8 bw);
void rtw_macid_ctl_set_vht_en(struct macid_ctl_t *macid_ctl, u8 id, u8 en);
void rtw_macid_ctl_set_rate_bmp0(struct macid_ctl_t *macid_ctl, u8 id, u32 bmp);
void rtw_macid_ctl_set_rate_bmp1(struct macid_ctl_t *macid_ctl, u8 id, u32 bmp);
void rtw_update_tx_rate_bmp(struct dvobj_priv *dvobj);

#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
void rtw_phydm_fill_desc_dpt(void *dm, u8 *desc, u8 dpt_lv);
#endif

#endif /* HOST_HAL_DM_PHYDM_OPS_TYPES_H */