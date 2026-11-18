/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_CFG_PHY_PARA_TYPES_H
#define HOST_HAL_DM_CFG_PHY_PARA_TYPES_H

#include <stdint.h>
#include <string.h>

#include "hal_halmac_phy_parameter.h"

typedef uint8_t u8;
typedef int8_t s8;
typedef uint32_t u32;

#ifndef RTW_HALMAC
#define RTW_HALMAC 1
#endif

enum hal_status {
	HAL_STATUS_SUCCESS = 0,
	HAL_STATUS_FAILURE = 1,
};

enum phydm_halmac_param {
	PHYDM_HALMAC_CMD_MAC_W8 = 0,
	PHYDM_HALMAC_CMD_MAC_W16 = 1,
	PHYDM_HALMAC_CMD_MAC_W32 = 2,
	PHYDM_HALMAC_CMD_BB_W8 = 3,
	PHYDM_HALMAC_CMD_BB_W16 = 4,
	PHYDM_HALMAC_CMD_BB_W32 = 5,
	PHYDM_HALMAC_CMD_RF_W = 6,
	PHYDM_HALMAC_CMD_DELAY_US = 7,
	PHYDM_HALMAC_CMD_DELAY_MS = 8,
	PHYDM_HALMAC_CMD_END = 0xFF,
};

enum rf_path {
	RF_PATH_A = 0,
	RF_PATH_B = 1,
	RF_PATH_C = 2,
	RF_PATH_D = 3,
};

struct dvobj_host {
	u8 _pad;
};

struct dm_struct {
	struct _adapter *adapter;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

typedef struct _adapter {
	struct dvobj_host dvobj;
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_dvobj(a) (&(a)->dvobj)

struct host_hal_dm_cfg_phy_para_trace {
	u8 cfg_calls;
	s8 stub_return;
	struct rtw_phy_parameter last_para;
};

extern struct host_hal_dm_cfg_phy_para_trace host_hal_dm_cfg_phy_para_trace;

void host_hal_dm_cfg_phy_para_reset(void);

int rtw_halmac_cfg_phy_para(struct dvobj_host *d, struct rtw_phy_parameter *para);
struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter);
void *rtw_rust_hal_dm_phydm_adapter(struct dm_struct *dm);

enum hal_status rtw_phydm_cfg_phy_para(struct dm_struct *p_dm_odm,
				       enum phydm_halmac_param config_type,
				       u32 offset, u32 data, u32 mask,
				       enum rf_path e_rf_path, u32 delay_time);

#endif /* HOST_HAL_DM_CFG_PHY_PARA_TYPES_H */