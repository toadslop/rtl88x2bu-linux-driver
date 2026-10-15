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

u32 rtw_rust_hal_phy_rf_max_reg(PADAPTER adapter)
{
	return GET_RF6052_REAL_MAX_REG(adapter);
}

#ifdef CONFIG_RF_SHADOW_RW
/* RF shadow table + PHY_RFShadowRead/Write/Refresh — rust/hal_phy.rs (W4-18) */
extern RF_SHADOW_T rtw_rust_rf_shadow[RF6052_MAX_PATH][RF6052_MAX_REG];

BOOLEAN
PHY_RFShadowCompare(
		PADAPTER		Adapter,
		enum rf_path		eRFPath,
		u32				Offset)
{
	u32	reg;
	/* Check if we need to check the register */
	if (rtw_rust_rf_shadow[eRFPath][Offset].Compare == _TRUE) {
		reg = rtw_hal_read_rfreg(Adapter, eRFPath, Offset, bRFRegOffsetMask);
		/* Compare shadow and real rf register for 20bits!! */
		if (rtw_rust_rf_shadow[eRFPath][Offset].Value != reg) {
			/* Locate error position. */
			rtw_rust_rf_shadow[eRFPath][Offset].ErrorOrNot = _TRUE;
		}
		return rtw_rust_rf_shadow[eRFPath][Offset].ErrorOrNot ;
	}
	return _FALSE;
}	/* PHY_RFShadowCompare */


void
PHY_RFShadowRecorver(
		PADAPTER		Adapter,
		enum rf_path		eRFPath,
		u32				Offset)
{
	/* Check if the address is error */
	if (rtw_rust_rf_shadow[eRFPath][Offset].ErrorOrNot == _TRUE) {
		/* Check if we need to recorver the register. */
		if (rtw_rust_rf_shadow[eRFPath][Offset].Recorver == _TRUE) {
			rtw_hal_write_rfreg(Adapter, eRFPath, Offset, bRFRegOffsetMask,
					    rtw_rust_rf_shadow[eRFPath][Offset].Value);
		}
	}

}	/* PHY_RFShadowRecorver */


void
PHY_RFShadowCompareAll(
		PADAPTER			Adapter)
{
	enum rf_path	eRFPath = RF_PATH_A;
	u32		Offset = 0, maxReg = GET_RF6052_REAL_MAX_REG(Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++)
			PHY_RFShadowCompare(Adapter, eRFPath, Offset);
	}

}	/* PHY_RFShadowCompareAll */


void
PHY_RFShadowRecorverAll(
		PADAPTER			Adapter)
{
	enum rf_path		eRFPath = RF_PATH_A;
	u32		Offset = 0, maxReg = GET_RF6052_REAL_MAX_REG(Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++)
			PHY_RFShadowRecorver(Adapter, eRFPath, Offset);
	}

}	/* PHY_RFShadowRecorverAll */


/* PHY_RFShadow*FlagSet* — rust/hal_phy.rs (W4-19) */

#endif /*CONFIG_RF_SHADOW_RW*/
