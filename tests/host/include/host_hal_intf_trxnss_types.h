// SPDX-License-Identifier: GPL-2.0
/* Host L2 types for W4-66 rtw_hal_trxnss_init. */

#ifndef HOST_HAL_INTF_TRXNSS_TYPES_H
#define HOST_HAL_INTF_TRXNSS_TYPES_H

#include <stdint.h>

typedef int8_t s8;
typedef uint8_t u8;

struct host_hal_trxnss_init_in {
	s8 hal_spec_tx_nss;
	s8 hal_spec_rx_nss;
	s8 reg_tx_nss;
	s8 reg_rx_nss;
	u8 max_tx_cnt;
	u8 rf_path;
	u8 hw_full_tx;
};

struct host_hal_trxnss_init_out {
	u8 tx_nss;
	u8 rx_nss;
	u8 txpath_cap_num_nss[4];
};

void host_hal_trxnss_init_common(const struct host_hal_trxnss_init_in *in,
				 struct host_hal_trxnss_init_out *out);

#endif
