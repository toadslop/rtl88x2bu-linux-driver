/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_DYNTPWR_TYPES_H
#define HOST_HAL_DM_DYNTPWR_TYPES_H

#include "host_types.h"

#define BIT(n) (1U << (n))

/* Matches include/hal_intf.h _CHIP_TYPE (subset for host L2). */
enum host_chip_type {
	HOST_NULL_CHIP_TYPE = 0,
	HOST_RTL8822B = 10,
};

#define HOST_ODM_BB_DYNAMIC_TXPWR BIT(2)
#define HOST_STA_DM_CTRL_ACTIVE BIT(0)

#define HOST_HALMAC_TX_DESC_DW5_OFF 20

struct host_dtp_info {
	u8 dyn_tx_power;
	u8 last_tx_power;
};

struct host_cmn_sta_dyntxpwr {
	u16 dm_ctrl;
	struct host_dtp_info dtp_stat;
};

#define HOST_DYNTPWR_MAX_STA 4

struct host_dyntxpwr_dvobj {
	u8 chip_type;
};

typedef struct host_dyntxpwr_dvobj host_dyntxpwr_dvobj;

typedef struct host_dyntxpwr_adapter host_dyntxpwr_adapter;

struct host_dm_dyntxpwr {
	host_dyntxpwr_adapter *adapter;
	u64 support_ability;
	struct host_cmn_sta_dyntxpwr *phydm_sta_info[HOST_DYNTPWR_MAX_STA];
	void (*fill_desc_dyntxpwr)(void *dm, u8 *desc, u8 dpt_lv);
};

struct host_dyntxpwr_adapter {
	host_dyntxpwr_dvobj dvobj;
	struct host_dm_dyntxpwr odmpriv;
};

struct host_hal_dm_dyntxpwr_trace {
	u8 err_logged;
	u8 odm_set_calls;
	u8 last_mac_id;
};

extern struct host_hal_dm_dyntxpwr_trace host_hal_dm_dyntxpwr_trace;

void host_hal_dm_dyntxpwr_reset(void);
void host_hal_dm_dyntxpwr_log_unsupported(void);
u8 host_get_chip_type(host_dyntxpwr_adapter *adapter);
struct host_dm_dyntxpwr *adapter_to_phydm_dyntxpwr(host_dyntxpwr_adapter *adapter);
void host_set_tx_desc_txpwr_ofset_8822b(u8 *txdesc, u8 value);
u8 host_get_tx_desc_txpwr_ofset_8822b(const u8 *txdesc);

void odm_set_dyntxpwr(void *dm_void, u8 *desc, u8 sta_id);

void rtw_phydm_fill_desc_dpt(void *dm, u8 *desc, u8 dpt_lv);
void rtw_phydm_set_dyntxpwr(host_dyntxpwr_adapter *adapter, u8 *desc, u8 mac_id);

#endif /* HOST_HAL_DM_DYNTPWR_TYPES_H */