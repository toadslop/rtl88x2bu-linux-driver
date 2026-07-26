// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle slice: core/rtw_wlan_util.c rate-classification helpers (T5/W3-08).
 */
#include <stdbool.h>

#include "host_wlan_util_types.h"

u8 WIFI_CCKRATES[] = {
	(IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK),
	(IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK)
};

u8 WIFI_OFDMRATES[] = {
	IEEE80211_OFDM_RATE_6MB,
	IEEE80211_OFDM_RATE_9MB,
	IEEE80211_OFDM_RATE_12MB,
	IEEE80211_OFDM_RATE_18MB,
	IEEE80211_OFDM_RATE_24MB,
	IEEE80211_OFDM_RATE_36MB,
	IEEE80211_OFDM_RATE_48MB,
	IEEE80211_OFDM_RATE_54MB
};

static u8 rtw_basic_rate_cck[4] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK
};

static u8 rtw_basic_rate_ofdm[3] = {
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static u8 rtw_basic_rate_mix[7] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

bool rtw_is_cck_rate(u8 rate)
{
	int i;

	for (i = 0; i < 4; i++)
		if ((WIFI_CCKRATES[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

bool rtw_is_ofdm_rate(u8 rate)
{
	int i;

	for (i = 0; i < 8; i++)
		if ((WIFI_OFDMRATES[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

bool rtw_is_basic_rate_cck(u8 rate)
{
	int i;

	for (i = 0; i < 4; i++)
		if ((rtw_basic_rate_cck[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

bool rtw_is_basic_rate_ofdm(u8 rate)
{
	int i;

	for (i = 0; i < 3; i++)
		if ((rtw_basic_rate_ofdm[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

bool rtw_is_basic_rate_mix(u8 rate)
{
	int i;

	for (i = 0; i < 7; i++)
		if ((rtw_basic_rate_mix[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

int cckrates_included(unsigned char *rate, int ratelen)
{
	int i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) == 2) || (((rate[i]) & 0x7f) == 4) ||
		    (((rate[i]) & 0x7f) == 11) || (((rate[i]) & 0x7f) == 22))
			return _TRUE;
	}
	return _FALSE;
}

int cckratesonly_included(unsigned char *rate, int ratelen)
{
	int i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
		    (((rate[i]) & 0x7f) != 11) && (((rate[i]) & 0x7f) != 22))
			return _FALSE;
	}
	return _TRUE;
}
