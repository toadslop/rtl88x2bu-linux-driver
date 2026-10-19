/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_HAL_SPEC_TYPES_H
#define HOST_HAL_COM_HAL_SPEC_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct host_hal_com_hal_spec_fixture {
	uint8_t macid_num;
	uint8_t macid_cap;
	uint8_t sec_cap;
	uint8_t sec_cam_ent_num;
	uint8_t rfpath_num_2g;
	uint8_t rfpath_num_5g;
	uint8_t rf_reg_path_num;
	uint8_t rf_reg_path_avail_num;
	uint8_t rf_reg_trx_path_bmp;
	uint8_t max_tx_cnt;
	uint8_t tx_nss_num;
	uint8_t rx_nss_num;
	uint8_t band_cap;
	uint8_t bw_cap;
	uint8_t proto_cap;
	uint8_t txgi_max;
	uint8_t txgi_pdbm;
	uint8_t wl_func;
	uint8_t tx_aclt_unit_factor;
	uint8_t rx_tsf_filter;
	uint8_t pg_txpwr_saddr;
	uint8_t pg_txgi_diff_factor;
} host_hal_com_hal_spec_fixture;

int dump_hal_spec_format(const host_hal_com_hal_spec_fixture *fx, char *buf,
			 size_t buflen);

#endif /* HOST_HAL_COM_HAL_SPEC_TYPES_H */
