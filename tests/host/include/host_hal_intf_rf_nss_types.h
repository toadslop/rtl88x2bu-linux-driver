/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_INTF_RF_NSS_TYPES_H
#define HOST_HAL_INTF_RF_NSS_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct host_hal_intf_rf_path_fixture {
	uint8_t ver_id_rf_type;
	uint8_t rf_reg_trx_path_bmp;
	uint8_t rf_reg_path_avail_num;
	uint8_t hal_spec_max_tx_cnt;
	uint8_t eeprom_trx_path_bmp;
	uint8_t eeprom_max_tx_cnt;
	uint8_t reg_trx_path_bmp;
	uint8_t tx_path_lmt;
	uint8_t rx_path_lmt;
	uint8_t hal_trx_path_bmp;
	uint8_t hal_max_tx_cnt;
	uint8_t hal_rf_type;
	int8_t NumTotalRFPath;
} host_hal_intf_rf_path_fixture;

typedef struct host_hal_intf_trx_nss_fixture {
	int8_t hal_spec_tx_nss;
	int8_t hal_spec_rx_nss;
	int8_t reg_tx_nss;
	int8_t reg_rx_nss;
	int8_t hal_tx_nss;
	int8_t hal_rx_nss;
} host_hal_intf_trx_nss_fixture;

int dump_rf_path_format(const host_hal_intf_rf_path_fixture *fx, char *buf,
			size_t buflen);
int dump_trx_nss_format(const host_hal_intf_trx_nss_fixture *fx, char *buf,
			size_t buflen);

#endif /* HOST_HAL_INTF_RF_NSS_TYPES_H */
