// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-18 PHY_RFShadowRead/Write/Refresh (from hal/hal_phy_rest.c). */

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

/* Test helpers: seed shadow without going through Write (for refresh vectors). */
void host_rf_shadow_seed(enum rf_path path, u32 offset, u32 value, u8 driver_write)
{
	RF_Shadow[path][offset].Value = value;
	RF_Shadow[path][offset].Driver_Write = driver_write;
	RF_Shadow[path][offset].Compare = _TRUE;
}

u32 host_rf_shadow_peek_value(enum rf_path path, u32 offset)
{
	return RF_Shadow[path][offset].Value;
}

u8 host_rf_shadow_peek_driver_write(enum rf_path path, u32 offset)
{
	return RF_Shadow[path][offset].Driver_Write;
}
