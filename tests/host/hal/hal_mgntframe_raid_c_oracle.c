// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-16 rtw_get_mgntframe_raid (from hal/hal_com_rest.c). */

#include <stdint.h>

typedef uint8_t u8;

struct host_hal_mgntframe_adapter {
	u8 hw_type;
};

#define HARDWARE_TYPE_RTL8188ES 2
#define HARDWARE_TYPE_RTL8192EE 3

#define WIRELESS_11B (1 << 0)
#define WIRELESS_11G (1 << 1)

#define RATEID_IDX_G 7
#define RATEID_IDX_B 8
#define RATR_INX_WIRELESS_G 5
#define RATR_INX_WIRELESS_B 6

static u8 host_get_hw_type(void *adapter)
{
	if (!adapter)
		return 0;
	return ((struct host_hal_mgntframe_adapter *)adapter)->hw_type;
}

#define IS_NEW_GENERATION_IC(_Adapter) \
	(host_get_hw_type(_Adapter) >= HARDWARE_TYPE_RTL8192EE)

uint8_t rtw_get_mgntframe_raid(void *adapter, unsigned char network_type)
{
	u8 raid;

	if (IS_NEW_GENERATION_IC(adapter)) {
		raid = (network_type & WIRELESS_11B) ? RATEID_IDX_B : RATEID_IDX_G;
	} else {
		raid = (network_type & WIRELESS_11B) ? RATR_INX_WIRELESS_B
						     : RATR_INX_WIRELESS_G;
	}
	return raid;
}
