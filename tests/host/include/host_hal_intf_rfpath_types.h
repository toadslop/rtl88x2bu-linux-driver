// SPDX-License-Identifier: GPL-2.0
/* Host L2 types for W4-65 rtw_hal_rfpath_init (common path). */

#ifndef HOST_HAL_INTF_RFPATH_TYPES_H
#define HOST_HAL_INTF_RFPATH_TYPES_H

#include <stdint.h>

typedef uint8_t u8;

struct host_hal_rfpath_init_in {
	u8 rf_reg_trx_path_bmp;
	u8 reg_trx_path_bmp;
	u8 eeprom_trx_path_bmp;
	u8 eeprom_max_tx_cnt;
	u8 hal_spec_max_tx_cnt;
	u8 tx_path_lmt;
	u8 rx_path_lmt;
};

struct host_hal_rfpath_init_out {
	u8 status;
	u8 hal_trx_path_bmp;
	u8 hal_rf_type;
	u8 NumTotalRFPath;
	u8 hal_max_tx_cnt;
	u8 fail_reason;
};

u8 host_hal_rfpath_init_common(const struct host_hal_rfpath_init_in *in,
			       struct host_hal_rfpath_init_out *out);

#endif
