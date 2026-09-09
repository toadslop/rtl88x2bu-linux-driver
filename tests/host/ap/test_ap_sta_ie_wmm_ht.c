// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_sta_ie_wmm_ht_types.h"

struct wmm_expect {
	int expect_flags;
	u32 expect_qos_option;
	u8 expect_qos_info;
	u8 expect_has_legacy_ac;
	u8 expect_uapsd_vo;
	u8 expect_uapsd_vi;
	u8 expect_uapsd_be;
	u8 expect_uapsd_bk;
	u8 expect_max_sp_len;
};

static int check_wmm(const char *name, u8 qos_en, u8 *ies, u16 ies_len,
		     const struct wmm_expect *exp)
{
	_adapter ad;
	struct sta_info sta;

	memset(&ad, 0, sizeof(ad));
	memset(&sta, 0, sizeof(sta));
	sta.flags = WLAN_STA_WME;
	ad.mlmepriv.qospriv.qos_option = qos_en;
	rtw_ap_parse_sta_wmm_ie(&ad, &sta, ies, ies_len);
	if (sta.flags != exp->expect_flags ||
	    sta.qos_option != exp->expect_qos_option ||
	    sta.qos_info != exp->expect_qos_info ||
	    sta.has_legacy_ac != exp->expect_has_legacy_ac ||
	    sta.uapsd_vo != exp->expect_uapsd_vo ||
	    sta.uapsd_vi != exp->expect_uapsd_vi ||
	    sta.uapsd_be != exp->expect_uapsd_be ||
	    sta.uapsd_bk != exp->expect_uapsd_bk ||
	    sta.max_sp_len != exp->expect_max_sp_len) {
		fprintf(stderr, "FAIL: %s\n", name);
		return -1;
	}
	return 0;
}

static int check_ht(const char *name, u8 ht_en, u8 *ht_cap, u8 ht_cap_len,
		    int expect_flags)
{
	_adapter ad;
	struct sta_info sta;
	struct rtw_ieee802_11_elems elems;

	memset(&ad, 0, sizeof(ad));
	memset(&sta, 0, sizeof(sta));
	memset(&elems, 0, sizeof(elems));
	sta.flags = WLAN_STA_HT;
	ad.mlmepriv.htpriv.ht_option = ht_en;
	elems.ht_capabilities = ht_cap;
	elems.ht_capabilities_len = ht_cap_len;
	rtw_ap_parse_sta_ht_ie(&ad, &sta, &elems);
	if (sta.flags != expect_flags) {
		fprintf(stderr, "FAIL: %s flags\n", name);
		return -1;
	}
	return 0;
}

int main(void)
{
	u8 wmm_all_uapsd[] = {0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x8f};
	u8 wmm_bk_on_be_off[] = {0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x47};
	u8 ht_cap[26];
	const u8 uapsd_ac = (u8)(BIT(0) | BIT(1));
	struct wmm_expect wmm_disabled = {
		.expect_flags = 0,
		.expect_qos_option = 0,
		.expect_qos_info = 0,
		.expect_has_legacy_ac = _TRUE,
	};
	struct wmm_expect wmm_present = {
		.expect_flags = (int)WLAN_STA_WME,
		.expect_qos_option = 1,
		.expect_qos_info = 0x8f,
		.expect_has_legacy_ac = _FALSE,
		.expect_uapsd_vo = uapsd_ac,
		.expect_uapsd_vi = uapsd_ac,
		.expect_uapsd_be = uapsd_ac,
		.expect_uapsd_bk = uapsd_ac,
		.expect_max_sp_len = 0,
	};
	struct wmm_expect wmm_bk_be_split = {
		.expect_flags = (int)WLAN_STA_WME,
		.expect_qos_option = 1,
		.expect_qos_info = 0x47,
		.expect_has_legacy_ac = _TRUE,
		.expect_uapsd_vo = uapsd_ac,
		.expect_uapsd_vi = uapsd_ac,
		.expect_uapsd_be = 0,
		.expect_uapsd_bk = uapsd_ac,
		.expect_max_sp_len = 2,
	};

	memset(ht_cap, 0, sizeof(ht_cap));
	if (check_wmm("wmm_qos_disabled", 0, wmm_all_uapsd, sizeof(wmm_all_uapsd),
		      &wmm_disabled) ||
	    check_wmm("wmm_present", 1, wmm_all_uapsd, sizeof(wmm_all_uapsd),
		      &wmm_present) ||
	    check_wmm("wmm_bk_on_be_off", 1, wmm_bk_on_be_off,
		      sizeof(wmm_bk_on_be_off), &wmm_bk_be_split) ||
	    check_ht("ht_disabled", 0, ht_cap, sizeof(ht_cap), 0) ||
	    check_ht("ht_present", 1, ht_cap, sizeof(ht_cap),
		     (int)(WLAN_STA_HT | WLAN_STA_WME)))
		return 1;
	printf("PASS: 5 wmm/ht vectors\n");
	return 0;
}
