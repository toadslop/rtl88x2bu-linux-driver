// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-18 PHY_RFShadowRead/Write/Refresh, W4-19 flag helpers,
 * W4-20 compare/recover. */

#include <stdint.h>

#define RF6052_MAX_REG_88E 0xFF
#define RF6052_MAX_REG_92C 0x7F
#define RF6052_MAX_REG                                                     \
	((RF6052_MAX_REG_88E > RF6052_MAX_REG_92C) ? RF6052_MAX_REG_88E :   \
						      RF6052_MAX_REG_92C)
#define RF6052_MAX_PATH 2
#define bRFRegOffsetMask 0xfffffU

enum rf_path { RF_PATH_A = 0, RF_PATH_B = 1 };

typedef uint8_t u8;
typedef uint32_t u32;

struct host_rf_shadow_adapter {
	u8 is_8188e;
};

#define IS_HARDWARE_TYPE_8188E(a) ((a)->is_8188e != 0)
#define GET_RF6052_REAL_MAX_REG(_Adapter)                                  \
	(IS_HARDWARE_TYPE_8188E(_Adapter) ? RF6052_MAX_REG_88E :           \
					    RF6052_MAX_REG_92C)

typedef struct {
	u32 Value;
	u8 Compare;
	u8 ErrorOrNot;
	u8 Recorver;
	u8 Driver_Write;
} RF_SHADOW_T;

static RF_SHADOW_T RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG];

#define _TRUE 1
#define _FALSE 0

u32 PHY_RFShadowRead(void *Adapter, enum rf_path eRFPath, u32 Offset)
{
	(void)Adapter;
	return RF_Shadow[eRFPath][Offset].Value;
}

void PHY_RFShadowWrite(void *Adapter, enum rf_path eRFPath, u32 Offset,
		       u32 Data)
{
	(void)Adapter;
	RF_Shadow[eRFPath][Offset].Value = (Data & bRFRegOffsetMask);
	RF_Shadow[eRFPath][Offset].Driver_Write = _TRUE;
}

void PHY_RFShadowRefresh(void *Adapter)
{
	enum rf_path eRFPath = RF_PATH_A;
	u32 Offset = 0;
	u32 maxReg = GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++) {
			RF_Shadow[eRFPath][Offset].Value = 0;
			RF_Shadow[eRFPath][Offset].Compare = _FALSE;
			RF_Shadow[eRFPath][Offset].Recorver = _FALSE;
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _FALSE;
			RF_Shadow[eRFPath][Offset].Driver_Write = _FALSE;
		}
	}
}

void PHY_RFShadowCompareFlagSet(void *a, enum rf_path p, u32 off, u8 t)
{
	(void)a;
	RF_Shadow[p][off].Compare = t;
}

void PHY_RFShadowRecorverFlagSet(void *a, enum rf_path p, u32 off, u8 t)
{
	(void)a;
	RF_Shadow[p][off].Recorver = t;
}

void PHY_RFShadowCompareFlagSetAll(void *Adapter)
{
	enum rf_path eRFPath = RF_PATH_A;
	u32 Offset = 0;
	u32 maxReg = GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++) {
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowCompareFlagSet(Adapter, eRFPath, Offset,
							   _FALSE);
			else
				PHY_RFShadowCompareFlagSet(Adapter, eRFPath, Offset,
							   _TRUE);
		}
	}
}

void PHY_RFShadowRecorverFlagSetAll(void *Adapter)
{
	enum rf_path eRFPath = RF_PATH_A;
	u32 Offset = 0;
	u32 maxReg = GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++) {
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowRecorverFlagSet(Adapter, eRFPath, Offset,
							    _FALSE);
			else
				PHY_RFShadowRecorverFlagSet(Adapter, eRFPath, Offset,
							    _TRUE);
		}
	}
}

#ifndef HOST_HAL_PHY_RF_SHADOW_L1_REF

u32 rtw_hal_read_rfreg(void *Adapter, enum rf_path eRFPath, u32 Offset,
		       u32 mask);
void rtw_hal_write_rfreg(void *Adapter, enum rf_path eRFPath, u32 Offset,
			 u32 mask, u32 data);

u8 PHY_RFShadowCompare(void *Adapter, enum rf_path eRFPath, u32 Offset)
{
	u32 reg;

	if (RF_Shadow[eRFPath][Offset].Compare == _TRUE) {
		reg = rtw_hal_read_rfreg(Adapter, eRFPath, Offset, bRFRegOffsetMask);
		if (RF_Shadow[eRFPath][Offset].Value != reg)
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _TRUE;
		return RF_Shadow[eRFPath][Offset].ErrorOrNot;
	}
	return _FALSE;
}

void PHY_RFShadowRecorver(void *Adapter, enum rf_path eRFPath, u32 Offset)
{
	if (RF_Shadow[eRFPath][Offset].ErrorOrNot == _TRUE) {
		if (RF_Shadow[eRFPath][Offset].Recorver == _TRUE) {
			rtw_hal_write_rfreg(Adapter, eRFPath, Offset,
					    bRFRegOffsetMask,
					    RF_Shadow[eRFPath][Offset].Value);
		}
	}
}

void PHY_RFShadowCompareAll(void *Adapter)
{
	enum rf_path eRFPath = RF_PATH_A;
	u32 Offset = 0;
	u32 maxReg = GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++)
			PHY_RFShadowCompare(Adapter, eRFPath, Offset);
	}
}

void PHY_RFShadowRecorverAll(void *Adapter)
{
	enum rf_path eRFPath = RF_PATH_A;
	u32 Offset = 0;
	u32 maxReg = GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++) {
		for (Offset = 0; Offset < maxReg; Offset++)
			PHY_RFShadowRecorver(Adapter, eRFPath, Offset);
	}
}

u32 rtw_rust_hal_phy_rf_max_reg(void *Adapter)
{
	return GET_RF6052_REAL_MAX_REG((struct host_rf_shadow_adapter *)Adapter);
}

u8 PHY_RFShadowTestPeekCompare(enum rf_path eRFPath, u32 Offset)
{
	return RF_Shadow[eRFPath][Offset].Compare;
}

u8 PHY_RFShadowTestPeekRecorver(enum rf_path eRFPath, u32 Offset)
{
	return RF_Shadow[eRFPath][Offset].Recorver;
}

u8 PHY_RFShadowTestPeekErrorOrNot(enum rf_path eRFPath, u32 Offset)
{
	return RF_Shadow[eRFPath][Offset].ErrorOrNot;
}

#endif /* HOST_HAL_PHY_RF_SHADOW_L1_REF */
