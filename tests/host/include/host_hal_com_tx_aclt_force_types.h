/* SPDX-License-Identifier: GPL-2.0 */
/* Host L2 fixture types for dump_tx_aclt_force_val (W4-39). */

#ifndef HOST_HAL_COM_TX_ACLT_FORCE_TYPES_H
#define HOST_HAL_COM_TX_ACLT_FORCE_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t tx_aclt_unit_factor;
	uint8_t en;
	uint32_t vo_vi;
	uint32_t be_bk;
} host_hal_com_tx_aclt_force_fixture;

int dump_tx_aclt_force_val_format(const host_hal_com_tx_aclt_force_fixture *fx,
				  char *buf, size_t buflen);

#endif
