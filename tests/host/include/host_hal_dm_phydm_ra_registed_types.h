/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_PHYDM_RA_REGISTED_TYPES_H
#define HOST_HAL_DM_PHYDM_RA_REGISTED_TYPES_H

#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef int8_t s8;

#define RTW_DBGDUMP 0

struct rssi_info_host {
	s8 rssi;
};

struct rtw_sta_cmn_ra {
	u8 mac_id;
	struct rssi_info_host rssi_stat;
};

struct sta_info {
	struct rtw_sta_cmn_ra cmn;
};

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

struct macid_ctl_t {
	u8 num;
};

struct dvobj_priv {
	struct macid_ctl_t macid_ctl;
};

typedef struct {
	struct dvobj_priv dvobj;
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)

struct dvobj_priv *adapter_to_dvobj(_adapter *adapter);
struct macid_ctl_t *dvobj_to_macidctl(struct dvobj_priv *dvobj);

struct host_hal_dm_phydm_ra_registed_trace {
	u8 warn_on_calls;
	u8 err_logged;
	u8 phydm_ra_calls;
	u8 last_mac_id;
	u8 last_rssi;
	u8 dump_sta_calls;
};

extern struct host_hal_dm_phydm_ra_registed_trace host_hal_dm_phydm_ra_registed_trace;

void host_hal_dm_phydm_ra_registed_reset(void);
void rtw_phydm_ra_registed(_adapter *adapter, struct sta_info *psta);

#define RTW_ERR(...) host_hal_dm_phydm_ra_registed_trace.err_logged = 1
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(adapter) (adapter)

void rtw_warn_on(int c);
void phydm_ra_registed(void *dm_void, u8 macid, u8 rssi_from_assoc);
void dump_sta_info(void *sel, struct sta_info *psta);

#endif /* HOST_HAL_DM_PHYDM_RA_REGISTED_TYPES_H */