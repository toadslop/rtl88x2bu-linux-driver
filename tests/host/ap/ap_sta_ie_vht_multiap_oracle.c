// SPDX-License-Identifier: GPL-2.0
#include "host_ap_sta_ie_vht_multiap_types.h"

typedef unsigned int uint;

static u8 MULTI_AP_OUI[4] = {0x50, 0x6F, 0x9A, 0x1B};

u8 rtw_get_multi_ap_ie_ext(const u8 *ies, int ies_len)
{
	u8 *ie;
	unsigned int ielen;
	u8 val = 0;

	ie = rtw_get_ie_ex(ies, (unsigned int)ies_len, WLAN_EID_VENDOR_SPECIFIC,
			   MULTI_AP_OUI, 4, NULL, &ielen);
	if (ielen < 9)
		goto exit;
	if (ie[6] != MULTI_AP_SUB_ELEM_TYPE)
		goto exit;
	val = ie[8];
exit:
	return val;
}
