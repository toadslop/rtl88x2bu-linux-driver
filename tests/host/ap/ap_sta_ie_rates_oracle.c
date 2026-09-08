// SPDX-License-Identifier: GPL-2.0
#include "host_ap_sta_ie_rates_types.h"

typedef int sint;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);

void UpdateBrateTblForSoftAP(u8 *bssrateset, u32 bssratelen)
{
	u8 i, rate;

	for (i = 0; i < bssratelen; i++) {
		rate = bssrateset[i] & 0x7f;
		switch (rate) {
		case IEEE80211_CCK_RATE_1MB:
		case IEEE80211_CCK_RATE_2MB:
		case IEEE80211_CCK_RATE_5MB:
		case IEEE80211_CCK_RATE_11MB:
			bssrateset[i] |= IEEE80211_BASIC_RATE_MASK;
			break;
		}
	}
}

int rtw_ies_get_supported_rate(u8 *ies, unsigned int ies_len, u8 *rate_set,
			       u8 *rate_num)
{
	struct { u8 rate; u8 existence; u8 basic; } tbl[12] = {
		{IEEE80211_CCK_RATE_1MB, _FALSE, _FALSE},
		{IEEE80211_CCK_RATE_2MB, _FALSE, _FALSE},
		{IEEE80211_CCK_RATE_5MB, _FALSE, _FALSE},
		{IEEE80211_CCK_RATE_11MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_6MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_9MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_12MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_18MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_24MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_36MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_48MB, _FALSE, _FALSE},
		{IEEE80211_OFDM_RATE_54MB, _FALSE, _FALSE},
	};
	u8 *ie, *p;
	unsigned int ie_len;
	int i, j;

	if (!rate_set || !rate_num)
		return _FAIL;

	*rate_num = 0;
	ie = rtw_get_ie(ies, _SUPPORTEDRATES_IE_, (int *)&ie_len, (int)ies_len);
	if (ie) {
		for (i = 0; i < 12; i++) {
			p = ie + 2;
			for (j = 0; j < (int)ie_len; j++) {
				if ((*p & 0x7f) == tbl[i].rate) {
					tbl[i].existence = _TRUE;
					if (*p & 0x80)
						tbl[i].basic = _TRUE;
				}
				p++;
			}
		}
	}
	ie = rtw_get_ie(ies, _EXT_SUPPORTEDRATES_IE_, (int *)&ie_len, (int)ies_len);
	if (ie) {
		for (i = 0; i < 12; i++) {
			p = ie + 2;
			for (j = 0; j < (int)ie_len; j++) {
				if ((*p & 0x7f) == tbl[i].rate) {
					tbl[i].existence = _TRUE;
					if (*p & 0x80)
						tbl[i].basic = _TRUE;
				}
				p++;
			}
		}
	}
	for (i = 0; i < 12; i++) {
		if (tbl[i].existence) {
			rate_set[*rate_num] = tbl[i].basic ?
				tbl[i].rate | IEEE80211_BASIC_RATE_MASK :
				tbl[i].rate;
			*rate_num += 1;
		}
	}
	return *rate_num ? _SUCCESS : _FAIL;
}
