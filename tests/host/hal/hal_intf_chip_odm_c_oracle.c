// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-36 chip_type → ODM IC lookup (hal/hal_intf.c table). */

#include "host_hal_intf_chip_odm_types.h"
#include "host_odm_phydm_types.h"

static const u32 chip_map[] = {
	0,
	ODM_RTL8188E,
	ODM_RTL8192E,
	ODM_RTL8812,
	ODM_RTL8821,
	ODM_RTL8723B,
	ODM_RTL8814A,
	ODM_RTL8703B,
	ODM_RTL8188F,
	ODM_RTL8188F,
	ODM_RTL8822B,
	ODM_RTL8723D,
	ODM_RTL8821C,
	ODM_RTL8710B,
	ODM_RTL8192F,
	ODM_RTL8822C,
	ODM_RTL8814B,
	ODM_RTL8723F,
	0,
};

u32 chip_type_to_odm_ic_type(u8 chip_type)
{
	if (chip_type >= HOST_MAX_CHIP_TYPE)
		return chip_map[HOST_MAX_CHIP_TYPE];
	return chip_map[chip_type];
}
