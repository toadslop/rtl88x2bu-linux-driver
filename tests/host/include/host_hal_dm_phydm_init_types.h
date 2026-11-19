/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_PHYDM_INIT_TYPES_H
#define HOST_HAL_DM_PHYDM_INIT_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef u8 boolean;

#define _FALSE 0
#define _TRUE 1
#define _SUCCESS 1
#define _FAIL 0

enum host_bb_path {
	HOST_BB_PATH_A = 0x00000001,
	HOST_BB_PATH_B = 0x00000002,
	HOST_BB_PATH_AB = (HOST_BB_PATH_A | HOST_BB_PATH_B),
	HOST_BB_PATH_AUTO = 0x000000ff,
};

enum odm_cmninfo_host {
	ODM_CMNINFO_FW_VER = 7,
	ODM_CMNINFO_FW_SUB_VER = 8,
};

enum phydm_init_result_host {
	PHYDM_INIT_SUCCESS = 0,
	PHYDM_INIT_FAIL_BBRF_REG_INVALID = 1,
};

struct dm_struct {
	struct _adapter *adapter;
};

struct hal_data_host {
	struct dm_struct odmpriv;
	u32 txpath;
	u32 rxpath;
	u32 txpath_nss[4];
	u8 firmware_version;
	u8 firmware_sub_version;
	u8 phydm_init_result;
};

struct dvobj_host {
	u8 _pad;
};

typedef struct _adapter {
	struct dvobj_host dvobj;
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_dvobj(a) (&(a)->dvobj)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_phydm_init_trace {
	u8 trx_calls;
	u32 last_tx;
	u32 last_rx;
	u32 last_tx_1ss;
	u8 trx_warn;
	u8 cmn_fw_ver_calls;
	u8 cmn_fw_sub_calls;
	u64 last_fw_ver;
	u64 last_fw_sub;
	u8 odm_dm_init_calls;
	u8 odm_dm_init_stub;
	u8 trx_stub_ok;
};

extern struct host_hal_dm_phydm_init_trace host_hal_dm_phydm_init_trace;

void host_hal_dm_phydm_init_reset(void);

boolean phydm_api_trx_mode(void *dm_void, u32 tx_path, u32 rx_path,
			   u32 tx_path_ctrl);
void odm_cmn_info_init(struct dm_struct *dm, u32 cmn_info, u64 value);
enum phydm_init_result_host odm_dm_init(struct dm_struct *dm);

void rtw_phydm_init(_adapter *adapter);

void rtw_warn_on(int cond);
#define RTW_ERR(...) host_hal_dm_phydm_init_trace.trx_warn = 1

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);
struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter);

u32 rtw_rust_hal_dm_phydm_init_txpath(_adapter *adapter);
u32 rtw_rust_hal_dm_phydm_init_rxpath(_adapter *adapter);
u32 rtw_rust_hal_dm_phydm_init_txpath_1ss(_adapter *adapter);
u8 rtw_rust_hal_dm_phydm_init_fw_ver(_adapter *adapter);
u8 rtw_rust_hal_dm_phydm_init_fw_sub_ver(_adapter *adapter);
void rtw_rust_hal_dm_phydm_init_set_result(_adapter *adapter, u8 result);
void rtw_rust_hal_dm_phydm_init_err_trx_config(const char *func, u32 txpath,
					       u32 rxpath, u32 txpath_1ss);

#endif /* HOST_HAL_DM_PHYDM_INIT_TYPES_H */