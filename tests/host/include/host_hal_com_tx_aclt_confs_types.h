/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 fixture types for dump_tx_aclt_confs (W4-40). */

#ifndef HOST_HAL_COM_TX_ACLT_CONFS_TYPES_H
#define HOST_HAL_COM_TX_ACLT_CONFS_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define HOST_TX_ACLT_CONF_NUM 3

typedef struct {
	uint8_t en;
	uint32_t vo_vi;
	uint32_t be_bk;
} host_hal_com_tx_aclt_conf_row;

typedef struct {
	uint8_t tx_aclt_unit_factor;
	host_hal_com_tx_aclt_conf_row confs[HOST_TX_ACLT_CONF_NUM];
} host_hal_com_tx_aclt_confs_fixture;

int dump_tx_aclt_confs_format(const host_hal_com_tx_aclt_confs_fixture *fx,
			      char *buf, size_t buflen);

#endif
