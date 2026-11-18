/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_PHYDM_TRX_CFG_TYPES_H
#define HOST_HAL_DM_PHYDM_TRX_CFG_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef u8 boolean;

#ifndef CONFIG_CTRL_TXSS_BY_TP
#define CONFIG_CTRL_TXSS_BY_TP 1
#endif

enum host_bb_path {
	HOST_BB_PATH_A = 0x00000001,
	HOST_BB_PATH_B = 0x00000002,
	HOST_BB_PATH_AB = (HOST_BB_PATH_A | HOST_BB_PATH_B),
};

#define _FALSE 0
#define _TRUE 1

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	struct dm_struct odmpriv;
};

struct dvobj_host {
	u8 _pad;
};

typedef struct {
	struct dvobj_host dvobj;
	struct hal_data_host HalData;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_dvobj(a) (&(a)->dvobj)
#define adapter_to_phydm(a) (&GET_HAL_DATA(a)->odmpriv)

struct host_hal_dm_phydm_trx_cfg_trace {
	u8 api_calls;
	u8 err_logged;
	u32 last_tx;
	u32 last_rx;
	u32 last_tx_1ss;
};

extern struct host_hal_dm_phydm_trx_cfg_trace host_hal_dm_phydm_trx_cfg_trace;

void host_hal_dm_phydm_trx_cfg_reset(void);
void host_trx_cfg_set_hal_paths(u32 txpath, u32 rxpath);
void host_trx_cfg_set_api_return(u8 ok);

#ifdef CONFIG_CTRL_TXSS_BY_TP
void rtw_hal_get_trx_path(struct dvobj_host *d, void *type, u32 *txpath,
			  u32 *rxpath);
boolean phydm_api_trx_mode(void *dm_void, u32 tx_path, u32 rx_path,
			   u32 tx_path_ctrl);
void rtw_phydm_trx_cfg(_adapter *adapter, bool tx_1ss);

#define RTW_ERR(...) host_hal_dm_phydm_trx_cfg_trace.err_logged = 1
#endif /* CONFIG_CTRL_TXSS_BY_TP */

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter);
struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter);
void rtw_rust_hal_dm_err_phydm_trx_cfg(void);

#endif /* HOST_HAL_DM_PHYDM_TRX_CFG_TYPES_H */