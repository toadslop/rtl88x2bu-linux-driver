// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-14 HAL_IsLegalChannel (from hal/hal_com_rest.c). */

#include <stdint.h>

#define _TRUE 1
#define _FALSE 0

typedef uint8_t BOOLEAN;
typedef uint8_t u8;
typedef uint32_t u32;

/* Minimal adapter stub: wireless_mode at offset 0 (host tests only). */
struct host_hal_legal_channel_adapter {
	u8 wireless_mode;
};

#define WIRELESS_MODE_24G 0x0b
#define WIRELESS_MODE_5G 0x54

static int is_supported_24g(u8 mode)
{
	return (mode & WIRELESS_MODE_24G) ? _TRUE : _FALSE;
}

static int is_supported_5g(u8 mode)
{
	return (mode & WIRELESS_MODE_5G) ? _TRUE : _FALSE;
}

BOOLEAN HAL_IsLegalChannel(void *adapter, u32 channel)
{
	BOOLEAN legal = _TRUE;
	u8 wireless_mode;

	if (!adapter)
		return _FALSE;

	wireless_mode = ((struct host_hal_legal_channel_adapter *)adapter)->wireless_mode;

	if (channel > 14) {
		if (is_supported_5g(wireless_mode) == _FALSE)
			legal = _FALSE;
	} else if (channel <= 14 && channel >= 1) {
		if (is_supported_24g(wireless_mode) == _FALSE)
			legal = _FALSE;
	} else {
		legal = _FALSE;
	}

	return legal;
}
