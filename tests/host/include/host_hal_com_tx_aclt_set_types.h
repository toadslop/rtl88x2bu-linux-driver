/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_TX_ACLT_SET_TYPES_H
#define HOST_HAL_COM_TX_ACLT_SET_TYPES_H

#define CONFIG_TX_AC_LIFETIME 1
#define TX_ACLT_CONF_NUM 3
/* Host fixture backing store (HAL still gates on TX_ACLT_CONF_NUM). */
#define TX_ACLT_CONF_STORE 256

typedef unsigned char u8;
typedef unsigned int u32;

struct tx_aclt_conf_t { u8 en; u32 vo_vi; u32 be_bk; };
struct hal_spec_t { u8 tx_aclt_unit_factor; };
struct dvobj_priv {
	u8 tx_aclt_flags;
	struct tx_aclt_conf_t tx_aclt_force_val;
	struct tx_aclt_conf_t tx_aclt_confs[TX_ACLT_CONF_STORE];
};
struct hal_data { struct hal_spec_t hal_spec; };
struct _adapter { struct hal_data hal; struct dvobj_priv dvobj; };

#define GET_HAL_SPEC(a) (&(a)->hal.hal_spec)
#define adapter_to_dvobj(a) (&(a)->dvobj)

void host_tx_aclt_reset(void);
struct _adapter *host_tx_aclt_adapter(void);
u8 rtw_rust_hal_com_tx_aclt_unit_factor(void *adapter);
struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_force_conf(void *adapter);
struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_conf_at(void *adapter, u8 idx);
u8 rtw_rust_hal_com_tx_aclt_flags(void *adapter);
u8 rtw_rust_hal_com_tx_aclt_read8(void *adapter, u32 addr);
u32 rtw_rust_hal_com_tx_aclt_read32(void *adapter, u32 addr);
void rtw_rust_hal_com_tx_aclt_write8(void *adapter, u32 addr, u8 val);
void rtw_rust_hal_com_tx_aclt_write32(void *adapter, u32 addr, u32 val);

#endif