/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_HAL_COM_RCR_AM_TYPES_H
#define HOST_HAL_COM_RCR_AM_TYPES_H

#include "host_hal_com_rcr_types.h"

/* HW_VAR_ON_RCR_AM / HW_VAR_OFF_RCR_AM — include/hal_intf.h (_HW_VARIABLES). */
#define HW_VAR_ON_RCR_AM 14
#define HW_VAR_OFF_RCR_AM 15

/* RCR_AM — include/hal_com_reg.h BIT(2). */
#define RCR_AM 4u

struct host_hal_rcr_am_trace {
	unsigned int add_count;
	unsigned int clear_count;
	u32 last_add_mask;
	u32 last_clear_mask;
};

#endif