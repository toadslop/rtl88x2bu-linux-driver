/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_FW_BCN_OFFLOAD_TYPES_H
#define HOST_HAL_COM_FW_BCN_OFFLOAD_TYPES_H

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef int bool;

struct _adapter;
struct dvobj_priv {
	u8 fw_bcn_offload;
	u8 vap_tbtt_rpt_map;
	u8 vap_map;
};

#define H2C_FW_BCN_OFFLOAD 0xBA
#define H2C_BCN_RSVDPAGE 0x09
#define H2C_BCN_OFFLOAD_LEN 1
#define H2C_BCN_RSVDPAGE_LEN 5
#define HAL_DEF_TX_PAGE_SIZE 25
#define MAX_BEACON_LEN 512
#define CONFIG_LIMITED_AP_NUM 4

#ifndef _TRUE
#define _TRUE 1
#define _FALSE 0
#endif

struct dvobj_priv *adapter_to_dvobj(struct _adapter *adapter);
void rtw_hal_set_fw_ap_bcn_offload_cmd(struct _adapter *adapter, bool fw_bcn_en,
				       u8 tbtt_rpt_map);
void rtw_hal_set_bcn_rsvdpage_loc_cmd(struct _adapter *adapter);
s32 rtw_hal_fill_h2c_cmd(struct _adapter *adapter, u8 cmd, unsigned int len, u8 *buf);
void rtw_hal_get_def_var(struct _adapter *adapter, int def, u8 *val);

#endif