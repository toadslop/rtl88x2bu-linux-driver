/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_TX_ACLT_SET_TYPES_H
#define HOST_HAL_COM_TX_ACLT_SET_TYPES_H

#define CONFIG_TX_AC_LIFETIME 1
#define TX_ACLT_CONF_NUM 3

typedef unsigned char u8;
typedef unsigned int u32;

struct tx_aclt_conf_t {
	u8 en;
	u32 vo_vi;
	u32 be_bk;
};

struct hal_spec_t {
	u8 tx_aclt_unit_factor;
};

struct dvobj_priv {
	struct tx_aclt_conf_t tx_aclt_force_val;
	struct tx_aclt_conf_t tx_aclt_confs[TX_ACLT_CONF_NUM];
	u8 oob_trap_3[sizeof(struct tx_aclt_conf_t)];
	u8 oob_trap_4[sizeof(struct tx_aclt_conf_t)];
};

struct hal_data {
	struct hal_spec_t hal_spec;
};

struct _adapter {
	struct hal_data hal;
	struct dvobj_priv dvobj;
};

#define GET_HAL_SPEC(adapter) (&(adapter)->hal.hal_spec)
#define adapter_to_dvobj(adapter) (&(adapter)->dvobj)

void host_tx_aclt_reset(void);
struct _adapter *host_tx_aclt_adapter(void);

#endif