// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-35 GET_RF6052_REAL_MAX_REG / rtw_rust_hal_phy_query_rf_max_reg. */

#include <stdint.h>

#include "host_hal_phy_rf_max_reg_types.h"

#define RF6052_MAX_REG_88E 0xFFu
#define RF6052_MAX_REG_92C 0x7Fu

#define HARDWARE_TYPE_RTL8188EE 0
#define HARDWARE_TYPE_RTL8188EU 1
#define HARDWARE_TYPE_RTL8188ES 2

static int is_hardware_type_8188e(uint8_t hw_type)
{
	return hw_type == HARDWARE_TYPE_RTL8188EE ||
	       hw_type == HARDWARE_TYPE_RTL8188EU ||
	       hw_type == HARDWARE_TYPE_RTL8188ES;
}

uint32_t rtw_rust_hal_phy_query_rf_max_reg(void *adapter)
{
	if (!adapter)
		return RF6052_MAX_REG_92C;
	const struct host_hal_phy_rf_max_reg_adapter *a = adapter;

	return is_hardware_type_8188e(a->hw_type) ? RF6052_MAX_REG_88E :
						   RF6052_MAX_REG_92C;
}
