// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_INTF_CHIP_RFTYPE_TYPES_H
#define HOST_HAL_INTF_CHIP_RFTYPE_TYPES_H

#include <stdint.h>

typedef uint8_t u8;

/* HAL_VERSION.RFType (HAL_RF_TYPE_E) → enum rf_type via version_id clamps. */
int host_chip_rftype_to_hal_rftype(u8 cv_rf_type, u8 limit);

#endif
