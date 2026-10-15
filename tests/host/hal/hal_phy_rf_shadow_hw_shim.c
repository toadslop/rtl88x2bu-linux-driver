// SPDX-License-Identifier: GPL-2.0
/* Stub rtw_hal_read/write_rfreg for W4-20 RF shadow compare/recover L2 tests. */

#include <stdint.h>
#include <string.h>

#define RF6052_MAX_REG_88E 0xFF
#define RF6052_MAX_REG_92C 0x7F
#define RF6052_MAX_REG                                                     \
	((RF6052_MAX_REG_88E > RF6052_MAX_REG_92C) ? RF6052_MAX_REG_88E :   \
						      RF6052_MAX_REG_92C)
#define RF6052_MAX_PATH 2

enum rf_path { RF_PATH_A = 0, RF_PATH_B = 1 };

typedef uint32_t u32;

static u32 hw_rf_reg[RF6052_MAX_PATH][RF6052_MAX_REG];

void host_rf_shadow_hw_reset(void)
{
	memset(hw_rf_reg, 0, sizeof(hw_rf_reg));
}

void host_rf_shadow_hw_set(enum rf_path path, u32 offset, u32 value)
{
	hw_rf_reg[path][offset] = value;
}

u32 host_rf_shadow_hw_get(enum rf_path path, u32 offset)
{
	return hw_rf_reg[path][offset];
}

u32 rtw_hal_read_rfreg(void *adapter, enum rf_path path, u32 offset, u32 mask)
{
	(void)adapter;
	return hw_rf_reg[path][offset] & mask;
}

void rtw_hal_write_rfreg(void *adapter, enum rf_path path, u32 offset,
			 u32 mask, u32 data)
{
	(void)adapter;
	hw_rf_reg[path][offset] =
		(hw_rf_reg[path][offset] & ~mask) | (data & mask);
}
