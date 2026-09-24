// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_ODM_RADAR_TXPWR_TYPES_H
#define HOST_ODM_RADAR_TXPWR_TYPES_H

#include "host_types.h"

typedef int BOOLEAN;
#define _TRUE 1
#define _FALSE 0

enum rtw_dfs_regd {
	RTW_DFS_REGD_NONE = 0,
	RTW_DFS_REGD_FCC = 1,
	RTW_DFS_REGD_MKK = 2,
	RTW_DFS_REGD_ETSI = 3,
	RTW_DFS_REGD_NUM,
};

enum phydm_dfs_region_domain {
	PHYDM_DFS_DOMAIN_UNKNOWN = 0,
	PHYDM_DFS_DOMAIN_FCC = 1,
	PHYDM_DFS_DOMAIN_MKK = 2,
	PHYDM_DFS_DOMAIN_ETSI = 3,
};

#define ODM_CMNINFO_DFS_REGION_DOMAIN 42

struct dvobj_priv;
typedef struct _adapter _adapter;

struct dm_struct {
	_adapter *adapter;
	u32 dfs_region_domain;
	u8 mock_radar_detect;
	u8 dfs_polling_ms;
};

typedef struct {
	struct dm_struct odmpriv;
} HAL_DATA_TYPE;

struct _adapter {
	HAL_DATA_TYPE HalData;
	struct dvobj_priv *dvobj;
};

struct rf_ctl_t {
	u8 dfs_region_domain;
};

struct dvobj_priv {
	struct rf_ctl_t rfctl;
	HAL_DATA_TYPE hal;
};

struct host_odm_radar_trace {
	int radar_reset, radar_disable, radar_enable, radar_detect, txpwr_calls;
	s16 txpwr_result;
	u8 txpwr_rfpath, txpwr_rate, txpwr_bw, txpwr_cch;
	u32 dfs_region_init;
};

extern struct host_odm_radar_trace host_odm_radar_trace;

#define GET_HAL_DATA(a) (&((a)->HalData))
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)
#define dvobj_to_phydm(d) (&((d)->hal.odmpriv))
#define dvobj_to_rfctl(d) (&((d)->rfctl))

void host_odm_radar_txpwr_reset(_adapter *adapter, struct dvobj_priv *dvobj);
u8 rtw_rfctl_get_dfs_domain(struct rf_ctl_t *rfctl);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u32 value);
s16 phy_get_txpwr_single_mbm(_adapter *adapter, u8 rfpath, u8 rs, u8 rate, u8 bw,
			     u8 cch, u8 offset, u8 dummy, u8 dummy2, void *dummy3);
u8 mgn_rate_to_rs(u8 rate);
void phydm_radar_detect_reset(struct dm_struct *dm);
void phydm_radar_detect_disable(struct dm_struct *dm);
void phydm_radar_detect_enable(struct dm_struct *dm);
BOOLEAN phydm_radar_detect(struct dm_struct *dm);
u8 phydm_dfs_polling_time(struct dm_struct *dm);

s16 rtw_odm_get_tx_power_mbm(struct dm_struct *dm, u8 rfpath, u8 rate, u8 bw, u8 cch);
void rtw_odm_radar_detect_reset(_adapter *adapter);
void rtw_odm_radar_detect_disable(_adapter *adapter);
void rtw_odm_radar_detect_enable(_adapter *adapter);
BOOLEAN rtw_odm_radar_detect(_adapter *adapter);
void rtw_odm_update_dfs_region(struct dvobj_priv *dvobj);
u8 rtw_odm_radar_detect_polling_int_ms(struct dvobj_priv *dvobj);

#endif
