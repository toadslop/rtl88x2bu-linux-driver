// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <string.h>

#include "host_ap_sta_ie_rates_types.h"

static int check_rates(const char *name, u8 *ies, u16 ies_len, u16 expect_status,
		       u32 expect_len, const char *expect_hex, int expect_flags)
{
	_adapter ad;
	struct sta_info sta;
	char got[32];
	u16 status;
	size_t i;

	memset(&ad, 0, sizeof(ad));
	memset(&sta, 0, sizeof(sta));
	ad.mlmepriv.cur_network.state = HOST_WIFI_AP_STATE;
	status = rtw_ap_parse_sta_supported_rates(&ad, &sta, ies, ies_len);
	if (status != expect_status) {
		fprintf(stderr, "FAIL: %s status\n", name);
		return -1;
	}
	if (expect_len) {
		if (sta.bssratelen != expect_len) {
			fprintf(stderr, "FAIL: %s ratelen\n", name);
			return -1;
		}
		got[0] = '\0';
		for (i = 0; i < sta.bssratelen; i++)
			sprintf(got + i * 2, "%02x", sta.bssrateset[i]);
		if (strcmp(got, expect_hex)) {
			fprintf(stderr, "FAIL: %s rates\n", name);
			return -1;
		}
	}
	if (sta.flags != expect_flags) {
		fprintf(stderr, "FAIL: %s flags\n", name);
		return -1;
	}
	return 0;
}

int main(void)
{
	u8 cck[] = {0x01, 0x04, 0x82, 0x84, 0x8b, 0x96};

	if (check_rates("rates_empty_fail", (u8 *)"", 0, _STATS_FAILURE_, 0, NULL, 0) ||
	    check_rates("rates_cck_ap", cck, sizeof(cck), _STATS_SUCCESSFUL_, 4,
			"82848b96", (int)WLAN_STA_NONERP))
		return 1;
	printf("PASS: 2 rates vectors\n");
	return 0;
}
