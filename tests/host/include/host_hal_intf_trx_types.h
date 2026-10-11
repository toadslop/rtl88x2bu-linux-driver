/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_INTF_TRX_TYPES_H
#define HOST_HAL_INTF_TRX_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct host_hal_intf_trx_fixture {
	uint8_t trx_path_bmp;
	uint8_t rf_type;
	uint8_t NumTotalRFPath;
	uint8_t max_tx_cnt;
	uint8_t tx_nss;
	uint8_t rx_nss;
	uint8_t txpath_cap_num_nss[4];
	uint8_t _pad[2];
	uint32_t txpath;
	uint32_t rxpath;
	uint32_t txpath_nss[4];
	uint8_t txpath_num_nss[4];
} host_hal_intf_trx_fixture;

int dump_hal_trx_mode_format(const host_hal_intf_trx_fixture *fx, char *buf,
			     size_t buflen);

#endif /* HOST_HAL_INTF_TRX_TYPES_H */
