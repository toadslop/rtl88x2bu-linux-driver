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
#define _RTW_AP_STA_IE_C_

#ifdef HOST_AP_STA_IE_TEST
#include "host_ap_sta_ie_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_STA_IE) || defined(HOST_AP_STA_IE_TEST)

void rtw_ap_parse_sta_capability(_adapter *adapter, struct sta_info *sta, u8 *cap)
{
	sta->capability = RTW_GET_LE16(cap);
	if (sta->capability & WLAN_CAPABILITY_SHORT_PREAMBLE)
		sta->flags |= WLAN_STA_SHORT_PREAMBLE;
	else
		sta->flags &= ~WLAN_STA_SHORT_PREAMBLE;
}

#endif /* !CONFIG_RUST_AP_STA_IE || HOST_AP_STA_IE_TEST */
