// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_sta_ie_vht_multiap_types.h"

static int check_vht(const char *name, u8 vht_en, u8 *cap, u8 cap_len, int expect_flags)
{
	_adapter ad;
	struct sta_info sta;
	struct rtw_ieee802_11_elems elems;

	memset(&ad, 0, sizeof(ad));
	memset(&sta, 0, sizeof(sta));
	memset(&elems, 0, sizeof(elems));
	sta.flags = WLAN_STA_VHT;
	ad.mlmepriv.vhtpriv.vht_option = vht_en;
	elems.vht_capabilities = cap;
	elems.vht_capabilities_len = cap_len;
	rtw_ap_parse_sta_vht_ie(&ad, &sta, &elems);
	if (sta.flags != expect_flags) {
		fprintf(stderr, "FAIL: %s\n", name);
		return -1;
	}
	return 0;
}

static int check_multi_ap(const char *name, u8 multi_ap, u8 *ies, int ies_len,
			  int expect_flags)
{
	_adapter ad;
	struct sta_info sta;

	memset(&ad, 0, sizeof(ad));
	memset(&sta, 0, sizeof(sta));
	sta.flags = WLAN_STA_MULTI_AP | WLAN_STA_WDS;
	ad.multi_ap = multi_ap;
	rtw_ap_parse_sta_multi_ap_ie(&ad, &sta, ies, ies_len);
	if (sta.flags != expect_flags) {
		fprintf(stderr, "FAIL: %s\n", name);
		return -1;
	}
	return 0;
}

int main(void)
{
	u8 vht_cap[VHT_CAP_IE_LEN];
	u8 multi_ap_bh[] = {0xdd, 0x07, 0x50, 0x6f, 0x9a, 0x1b, 0x06, 0x01, 0x80};

	memset(vht_cap, 0, sizeof(vht_cap));
	if (check_vht("vht_disabled", 0, vht_cap, sizeof(vht_cap), 0) ||
	    check_vht("vht_present", 1, vht_cap, sizeof(vht_cap), (int)WLAN_STA_VHT) ||
	    check_multi_ap("multi_ap_disabled", 0, multi_ap_bh, (int)sizeof(multi_ap_bh),
			   (int)WLAN_STA_WDS) ||
	    check_multi_ap("multi_ap_backhaul", MULTI_AP_BACKHAUL_BSS, multi_ap_bh,
			   (int)sizeof(multi_ap_bh),
			   (int)(WLAN_STA_MULTI_AP | WLAN_STA_WDS)) ||
	    check_multi_ap("multi_ap_fronthaul", MULTI_AP_FRONTHAUL_BSS, multi_ap_bh,
			   (int)sizeof(multi_ap_bh),
			   (int)(WLAN_STA_MULTI_AP | WLAN_STA_WDS)))
		return 1;
	printf("PASS: 5 vht/multi-ap vectors\n");
	return 0;
}
