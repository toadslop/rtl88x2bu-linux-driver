// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-28 usb_vendor_wvalue_fixup (extracted from hal/hal_hci/hal_usb.c). */

#include <stdint.h>

typedef uint16_t u16;

u16 usb_vendor_wvalue_fixup(u16 wvalue)
{
/* WLANON PAGE0_REG needs to add an offset 0x8000 */
#if defined(CONFIG_RTL8710B)
	if (wvalue < 0x0100)
		wvalue |= 0x8000;
#endif
	return wvalue;
}
