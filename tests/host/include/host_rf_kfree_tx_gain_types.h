/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Host L2 types for kfree TX gain offset helpers (W3-59 PR1).
 */
#ifndef HOST_RF_KFREE_TX_GAIN_TYPES_H
#define HOST_RF_KFREE_TX_GAIN_TYPES_H

#include "host_rf_types.h"

#define CONFIG_RF_POWER_TRIM 1
#define KFREE_FLAG_ON BIT(0)

#define IS_HARDWARE_TYPE_8723D(_Adapter) (0)

struct kfree_data_t {
	u8 flag;
	s8 bb_gain[BB_GAIN_NUM][RF_PATH_MAX];
};

struct hal_data_t {
	struct kfree_data_t kfree_data;
};

struct _adapter {
	struct hal_data_t hal_data;
};

typedef struct _adapter _adapter;

#define GET_HAL_DATA(_adapter) (&((_adapter)->hal_data))
#define GET_KFREE_DATA(_adapter) (&(GET_HAL_DATA(_adapter)->kfree_data))

#define RTW_INFO(...) do { } while (0)

s8 rtw_rf_get_kfree_tx_gain_offset(struct _adapter *padapter, u8 path, u8 ch);

#endif /* HOST_RF_KFREE_TX_GAIN_TYPES_H */
