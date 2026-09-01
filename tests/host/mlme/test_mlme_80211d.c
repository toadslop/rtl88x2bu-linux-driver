// SPDX-License-Identifier: GPL-2.0
/* W3-66 host L2 oracle for process_80211d (core/rtw_mlme_rest.c). */
#include <stdio.h>
#include <string.h>
#include "host_mlme_80211d_types.h"

struct case_vec {
	const char *name;
	u8 enable, done, mode;
	const u8 *ie;
	size_t ie_len;
	const u8 *sta_ch;
	int sta_n;
	const u8 *expect_ch;
	int expect_n;
	u8 expect_done;
};

static void fill_bss(WLAN_BSSID_EX *b, const u8 *ie, size_t n)
{
	memset(b, 0, sizeof(*b));
	b->IELength = _FIXED_IE_LENGTH_;
	if (ie && n) {
		memcpy(b->IEs + _FIXED_IE_LENGTH_, ie, n);
		b->IELength += (u32)n;
	}
}

static void fill_sta(_adapter *a, const u8 *ch, int n)
{
	for (int i = 0; i < n; i++) {
		a->rfctl.channel_set[i].ChannelNum = ch[i];
		a->rfctl.channel_set[i].flags = 0;
	}
}

static int chset_match(const RT_CHANNEL_INFO *cs, const u8 *exp, int n)
{
	for (int i = 0; i < n; i++) {
		if (cs[i].ChannelNum != exp[i])
			return -1;
	}
	if (cs[n].ChannelNum != 0)
		return -1;
	return 0;
}

static int run_case(const struct case_vec *v)
{
	_adapter a;
	WLAN_BSSID_EX b;

	memset(&a, 0, sizeof(a));
	a.registrypriv.enable80211d = v->enable;
	a.mlmeextpriv.update_channel_plan_by_ap_done = v->done;
	a.registrypriv.wireless_mode = v->mode;
	fill_sta(&a, v->sta_ch, v->sta_n);
	fill_bss(&b, v->ie, v->ie_len);
	process_80211d(&a, &b);
	if (a.mlmeextpriv.update_channel_plan_by_ap_done != v->expect_done)
		return -1;
	if (v->expect_n >= 0 && chset_match(a.rfctl.channel_set, v->expect_ch, v->expect_n))
		return -1;
	return 0;
}

int main(void)
{
	static const u8 us2g[] = {0x07, 0x09, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e};
	static const u8 us24g[] = {0x07, 0x0c, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e, 0x24, 0x04, 0x1e};
	static const u8 short_ie[] = {0x07, 0x03, 0x55, 0x53, 0x20};
	static const u8 sta246[] = {1, 6, 11};
	static const u8 sta246_5g[] = {1, 6, 11, 36, 44};
	static const u8 exp11[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	static const u8 exp11_5g[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 36, 40, 44, 48};
	static const struct case_vec cases[] = {
		{"disabled", 0, 0, WIRELESS_11G, us2g, sizeof(us2g), NULL, 0, NULL, -1, 0},
		{"already_done", 1, 1, WIRELESS_11G, us2g, sizeof(us2g), sta246, 3, sta246, 3, 1},
		{"no_country_ie", 1, 0, WIRELESS_11G, NULL, 0, sta246, 3, sta246, 3, 0},
		{"short_country_ie", 1, 0, WIRELESS_11G, short_ie, sizeof(short_ie), sta246, 3, sta246, 3, 0},
		{"merge_2g_11g", 1, 0, WIRELESS_11G, us2g, sizeof(us2g), sta246, 3, exp11, 11, 1},
		{"merge_dualband_11ag", 1, 0, WIRELESS_11G | WIRELESS_11A, us24g,
		 sizeof(us24g), sta246_5g, 5, exp11_5g, 15, 1},
	};
	int bad = 0;

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
		bad += run_case(&cases[i]) ?
			(fprintf(stderr, "FAIL %s\n", cases[i].name), 1) :
			(printf("PASS %s\n", cases[i].name), 0);
	if (!bad)
		printf("PASS %zu vectors (oracle: core/rtw_mlme_rest.c)\n",
		       sizeof(cases) / sizeof(cases[0]));
	return bad ? 1 : 0;
}
