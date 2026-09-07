// SPDX-License-Identifier: GPL-2.0
/* Minimal rate oracle for W3-72 band IE host tests. */
#include "host_mlme_ext_band_ie_types.h"

extern u8 WIFI_CCKRATES[];
extern u8 WIFI_OFDMRATES[];

void rtw_set_supported_rate(u8 *SupportedRates, unsigned int mode)
{
	memset(SupportedRates, 0, NDIS_802_11_LENGTH_RATES_EX);

	switch (mode) {
	case WIRELESS_11B:
		memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		break;
	case WIRELESS_11G:
	case WIRELESS_11A:
	case WIRELESS_11_5N:
	case WIRELESS_11A_5N:
	case WIRELESS_11_5AC:
		memcpy(SupportedRates, WIFI_OFDMRATES, IEEE80211_NUM_OFDM_RATESLEN);
		break;
	case WIRELESS_11BG:
	case WIRELESS_11G_24N:
	case WIRELESS_11_24N:
	case WIRELESS_11BG_24N:
		memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		memcpy(SupportedRates + IEEE80211_CCK_RATE_LEN, WIFI_OFDMRATES,
		       IEEE80211_NUM_OFDM_RATESLEN);
		break;
	}
}
