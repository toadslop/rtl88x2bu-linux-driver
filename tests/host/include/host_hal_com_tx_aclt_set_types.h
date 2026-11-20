/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_TX_ACLT_SET_TYPES_H
#define HOST_HAL_COM_TX_ACLT_SET_TYPES_H

#define CONFIG_TX_AC_LIFETIME 1
#define TX_ACLT_CONF_NUM 3
/* Host fixture backing store (HAL still gates on TX_ACLT_CONF_NUM). */
#define TX_ACLT_CONF_STORE 256
#define TX_ACLT_CONF_DEFAULT 0
#define TX_ACLT_CONF_AP_M2U 1
#define REG_LIFETIME_EN 0x0426u
#define REG_PKT_LIFE_TIME 0x04C0u

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct tx_aclt_conf_t { u8 en; u32 vo_vi; u32 be_bk; };
struct hal_spec_t { u8 tx_aclt_unit_factor; };
struct dvobj_priv {
	u8 tx_aclt_flags;
	struct tx_aclt_conf_t tx_aclt_force_val;
	struct tx_aclt_conf_t tx_aclt_confs[TX_ACLT_CONF_STORE];
	u8 host_ap_m2u_num;
	u8 host_sta_op_num;
};
struct hal_data { struct hal_spec_t hal_spec; };
struct _adapter { struct hal_data hal; struct dvobj_priv dvobj; };

#define GET_HAL_SPEC(a) (&(a)->hal.hal_spec)
#define adapter_to_dvobj(a) (&(a)->dvobj)

struct host_tx_aclt_reg_trace {
	u8 reg_lt_en;
	u32 reg_lt;
	u8 write8_count;
	u8 write32_count;
	u8 last_lt_en;
	u32 last_lt;
};

extern struct host_tx_aclt_reg_trace host_tx_aclt_reg_trace;

void host_tx_aclt_reset(void);
void rtw_rust_hal_com_tx_aclt_compute_lt(void *adapter, u8 *lt_en, u16 *lt_vo_vi,
					 u16 *lt_be_bk);
u8 rtw_rust_hal_com_tx_aclt_unit_factor(void *adapter);
struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_force_conf(void *adapter);
struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_conf_at(void *adapter, u8 idx);
u8 rtw_rust_hal_com_tx_aclt_flags(void *adapter);
u8 rtw_rust_hal_com_tx_aclt_read8(void *adapter, u32 addr);
u32 rtw_rust_hal_com_tx_aclt_read32(void *adapter, u32 addr);
void rtw_rust_hal_com_tx_aclt_write8(void *adapter, u32 addr, u8 val);
void rtw_rust_hal_com_tx_aclt_write32(void *adapter, u32 addr, u32 val);

#endif