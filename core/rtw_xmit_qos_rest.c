/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _RTW_XMIT_QOS_REST_C_

#ifdef HOST_XMIT_QOS_TEST
#include "host_xmit_qos_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_XMIT_QOS_TEST)

u8 qos_acm(u8 acm_mask, u8 priority)
{
	u8 change_priority = priority;

	switch (priority) {
	case 0:
	case 3:
		if (acm_mask & BIT(1))
			change_priority = 1;
		break;
	case 1:
	case 2:
		break;
	case 4:
	case 5:
		if (acm_mask & BIT(2))
			change_priority = 0;
		break;
	case 6:
	case 7:
		if (acm_mask & BIT(3))
			change_priority = 5;
		break;
	default:
		break;
	}
	return change_priority;
}

u8 tos_to_up(u8 tos)
{
	u8 up = 0;
	u8 dscp;
	u8 mode = CONFIG_RTW_UP_MAPPING_RULE;

	if (mode == 0)
		return tos >> 5;

	dscp = (tos >> 2);
	if (dscp == 0)
		up = 0;
	else if (dscp >= 1 && dscp <= 9)
		up = 1;
	else if (dscp >= 10 && dscp <= 16)
		up = 2;
	else if (dscp >= 17 && dscp <= 23)
		up = 3;
	else if (dscp >= 24 && dscp <= 31)
		up = 4;
	else if (dscp >= 33 && dscp <= 40)
		up = 5;
	else if ((dscp >= 41 && dscp <= 47) || (dscp == 32))
		up = 6;
	else if (dscp >= 48 && dscp <= 63)
		up = 7;
	return up;
}

#endif /* !CONFIG_RUST || HOST_XMIT_QOS_TEST */
