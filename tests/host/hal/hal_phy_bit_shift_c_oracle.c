// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-09 PHY_CalculateBitShift (extracted from hal/hal_phy.c). */

#include <stdint.h>

uint32_t PHY_CalculateBitShift(uint32_t bit_mask)
{
	uint32_t i;

	for (i = 0; i <= 31; i++) {
		if (((bit_mask >> i) & 0x1) == 1)
			break;
	}

	return i;
}
