// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_DM_HALMAC_DM_LEAF_TYPES_H
#define HOST_HAL_DM_HALMAC_DM_LEAF_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef u8 boolean;

#ifndef RTW_HALMAC
#define RTW_HALMAC 1
#endif

#define _FALSE 0
#define _TRUE 1

enum hal_status {
	HAL_STATUS_SUCCESS = 0,
	HAL_STATUS_FAILURE = 1,
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
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_halmac_dm_leaf_trace {
	u8 rfe_calls;
	u8 last_gpio;
	u8 rfe_stub_return;
	u8 iqk_calls;
	u8 last_clear;
	u8 last_segment;
	s8 iqk_stub_return;
};

extern struct host_hal_dm_halmac_dm_leaf_trace host_hal_dm_halmac_dm_leaf_trace;

void host_hal_dm_halmac_dm_leaf_reset(void);

int rtw_halmac_rfe_ctrl_cfg(struct dvobj_host *d, u8 gpio);
int rtw_halmac_iqk(struct dvobj_host *d, u8 clear, u8 segment);

boolean rtw_phydm_rfe_ctrl_gpio(_adapter *adapter, u8 gpio_num);
enum hal_status rtw_phydm_fw_iqk(struct dm_struct *p_dm_odm, u8 clear, u8 segment);

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);
struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter);
void *rtw_rust_hal_dm_phydm_adapter(struct dm_struct *dm);

#endif /* HOST_HAL_DM_HALMAC_DM_LEAF_TYPES_H */