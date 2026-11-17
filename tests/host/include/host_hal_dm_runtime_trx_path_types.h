/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_DM_RUNTIME_TRX_PATH_TYPES_H
#define HOST_HAL_DM_RUNTIME_TRX_PATH_TYPES_H

#include <stdint.h>

/*
 * Default 8822BU module build (Makefile CONFIG_RTL8822B = y). Optional
 * CONFIG_RTW_TX_NPATH_EN / CONFIG_RTW_PATH_DIV match drv_conf.h — undefined on
 * the stock 8822BU path unless a host Makefile passes -D flags.
 */
#ifndef CONFIG_RTL8822B
#define CONFIG_RTL8822B 1
#endif

typedef uint8_t u8;
typedef uint32_t u32;

#define BIT(n) (1U << (n))
#define RF_PATH_MAX 4

enum host_bb_path {
	HOST_BB_PATH_NON = 0,
	HOST_BB_PATH_A = 0x00000001,
	HOST_BB_PATH_B = 0x00000002,
	HOST_BB_PATH_AB = (HOST_BB_PATH_A | HOST_BB_PATH_B),
	HOST_BB_PATH_AUTO = 0xff
};

#define _FAIL 0
#define _SUCCESS 1

struct dm_struct {
	u8 _pad;
};

struct hal_data_host {
	u8 trx_path_bmp;
	u8 max_tx_cnt;
	u32 txpath;
	u32 rxpath;
	u32 txpath_nss[4];
	u8 txpath_num_nss[4];
	struct dm_struct odmpriv;
};

struct registry_host {
	u8 path_div;
	u8 tx_npath;
};

struct dvobj_host {
	u8 _pad;
};

typedef struct {
	struct dvobj_host dvobj;
	struct hal_data_host HalData;
	struct registry_host registrypriv;
} _adapter;

#define GET_HAL_DATA(a) (&(a)->HalData)
#define adapter_to_dvobj(a) (&(a)->dvobj)
#define GET_HAL_TX_PATH_BMP(a) (((GET_HAL_DATA(a)->trx_path_bmp) & 0xF0) >> 4)

struct host_hal_dm_runtime_trx_trace {
	u8 dump_calls;
	u8 warn_on_calls;
};

extern struct host_hal_dm_runtime_trx_trace host_hal_dm_runtime_trx_trace;

void host_hal_dm_runtime_trx_reset(void);
void host_runtime_trx_set_hal_get_paths(u32 txpath, u32 rxpath);

void rtw_hal_get_trx_path(struct dvobj_host *d, void *type, u32 *txpath, u32 *rxpath);
void tx_path_nss_set_default(u32 txpath_nss[], u8 txpath_num_nss[], u8 txpath);
void dump_hal_runtime_trx_mode(void *sel, _adapter *adapter);
void rtw_warn_on(int c);

struct dvobj_host *rtw_rust_hal_dm_adapter_to_dvobj(_adapter *adapter);
u8 rtw_hal_runtime_trx_path_decision(_adapter *adapter);

#endif