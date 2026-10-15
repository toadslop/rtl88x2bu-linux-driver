/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _HAL_PHY_REST_C_

#include <drv_types.h>

/* PHY_CalculateBitShift — ported to rust/hal_phy.rs (W4-09) */
/* PHY_RFShadow* — rust/hal_phy.rs (W4-18..W4-20) */

u32 rtw_rust_hal_phy_query_rf_max_reg(PADAPTER adapter)
{
	return GET_RF6052_REAL_MAX_REG(adapter);
}
