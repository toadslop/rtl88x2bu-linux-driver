// SPDX-License-Identifier: GPL-2.0
/*
 * C oracle slice: core/rtw_wlan_util.c rate-classification helpers (T5/W3-08).
 */
#include <stdbool.h>
#include <string.h>

#include "host_chplan_types.h"
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

/* ----- ratetbl conversion helpers (W3-09) ----- */

#define NumRates 13
#define _6M_RATE_ 4

typedef struct {
	u8 oper_ch;
	struct {
		u8 basicrate[NumRates];
		u8 datarate[NumRates];
	} mlmeextpriv;
} host_wlan_adapter;

unsigned char host_ratetbl_val_2wifirate(unsigned char rate)
{
	unsigned char val = 0;

	switch (rate & 0x7f) {
	case 0:
		val = IEEE80211_CCK_RATE_1MB;
		break;
	case 1:
		val = IEEE80211_CCK_RATE_2MB;
		break;
	case 2:
		val = IEEE80211_CCK_RATE_5MB;
		break;
	case 3:
		val = IEEE80211_CCK_RATE_11MB;
		break;
	case 4:
		val = IEEE80211_OFDM_RATE_6MB;
		break;
	case 5:
		val = IEEE80211_OFDM_RATE_9MB;
		break;
	case 6:
		val = IEEE80211_OFDM_RATE_12MB;
		break;
	case 7:
		val = IEEE80211_OFDM_RATE_18MB;
		break;
	case 8:
		val = IEEE80211_OFDM_RATE_24MB;
		break;
	case 9:
		val = IEEE80211_OFDM_RATE_36MB;
		break;
	case 10:
		val = IEEE80211_OFDM_RATE_48MB;
		break;
	case 11:
		val = IEEE80211_OFDM_RATE_54MB;
		break;
	default:
		break;
	}
	return val;
}

int host_is_basicrate(host_wlan_adapter *padapter, unsigned char rate)
{
	int i;
	unsigned char val;

	for (i = 0; i < NumRates; i++) {
		val = padapter->mlmeextpriv.basicrate[i];
		if ((val != 0xff) && (val != 0xfe)) {
			if (rate == host_ratetbl_val_2wifirate(val))
				return _TRUE;
		}
	}
	return _FALSE;
}

unsigned int host_ratetbl2rateset(host_wlan_adapter *padapter,
				  unsigned char *rateset)
{
	int i;
	unsigned char rate;
	unsigned int len = 0;

	for (i = 0; i < NumRates; i++) {
		rate = padapter->mlmeextpriv.datarate[i];

		if (padapter->oper_ch > 14 && rate < _6M_RATE_)
			continue;

		switch (rate) {
		case 0xff:
			return len;
		case 0xfe:
			continue;
		default:
			rate = host_ratetbl_val_2wifirate(rate);
			if (host_is_basicrate(padapter, rate) == _TRUE)
				rate |= IEEE80211_BASIC_RATE_MASK;
			rateset[len] = rate;
			len++;
			break;
		}
	}
	return len;
}

/* ----- network type + MCS helpers (W3-09b) ----- */

typedef struct {
	char _pad0[0x844];
	u8 cur_channel;
	char _pad1[0xc58 - 0x844 - 1];
	u8 ht_enable;
	char _pad2[0xc66 - 0xc58 - 1];
	u8 vht_enable;
} host_network_adapter;

u8 judge_network_type(host_network_adapter *padapter, unsigned char *rate,
		      int ratelen)
{
	u8 network_type = 0;

	if (padapter->cur_channel > 14) {
		if (padapter->vht_enable)
			network_type = WIRELESS_11AC;
		else if (padapter->ht_enable)
			network_type = WIRELESS_11_5N;

		network_type |= WIRELESS_11A;
	} else {
		if (padapter->ht_enable)
			network_type = WIRELESS_11_24N;

		if (cckratesonly_included(rate, ratelen) == _TRUE)
			network_type |= WIRELESS_11B;
		else if (cckrates_included(rate, ratelen) == _TRUE)
			network_type |= WIRELESS_11BG;
		else
			network_type |= WIRELESS_11G;
	}

	return network_type;
}

void get_rate_set(host_wlan_adapter *padapter, unsigned char *pbssrate,
		  int *bssrate_len)
{
	unsigned char supportedrates[NumRates];

	memset(supportedrates, 0, sizeof(supportedrates));
	*bssrate_len = (int)host_ratetbl2rateset(padapter, supportedrates);
	memcpy(pbssrate, supportedrates, (size_t)*bssrate_len);
}

void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask)
{
	u8 mcs_rate_1r = (u8)(mask & 0xff);
	u8 mcs_rate_2r = (u8)((mask >> 8) & 0xff);
	u8 mcs_rate_3r = (u8)((mask >> 16) & 0xff);
	u8 mcs_rate_4r = (u8)((mask >> 24) & 0xff);

	mcs_set[0] &= mcs_rate_1r;
	mcs_set[1] &= mcs_rate_2r;
	mcs_set[2] &= mcs_rate_3r;
	mcs_set[3] &= mcs_rate_4r;
}
