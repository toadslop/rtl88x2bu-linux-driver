// SPDX-License-Identifier: GPL-2.0
#ifndef HOST_HAL_INTF_CHIP_ODM_TYPES_H
#define HOST_HAL_INTF_CHIP_ODM_TYPES_H

#include <stdint.h>

/* Mirrors MAX_CHIP_TYPE in include/hal_intf.h (_CHIP_TYPE sentinel = 18). */
#define HOST_MAX_CHIP_TYPE 18

typedef uint8_t u8;
typedef uint32_t u32;

u32 chip_type_to_odm_ic_type(u8 chip_type);

#endif
