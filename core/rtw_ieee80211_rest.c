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
#define _IEEE80211_REST_C_

#ifdef HOST_IEEE80211_REST_TEST
#include "host_ieee80211_types.h"
#define _rtw_memmove memmove
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_IEEE80211_REST_TEST)
/*
 * W3-26: rate classification helpers extracted from core/rtw_ieee80211.c.
 * C oracle for host L2; kernel builds use rust/rtw_ieee80211_rest.rs when
 * CONFIG_RUST.
 */

#ifdef HOST_IEEE80211_REST_TEST
typedef int sint;
typedef unsigned int uint;

extern u8 WIFI_CCKRATES[];
extern u8 WIFI_OFDMRATES[];

static uint rtw_is_cckrates_included(u8 *rate)
{
	u32 i = 0;

	while (rate[i] != 0) {
		if ((((rate[i]) & 0x7f) == 2) || (((rate[i]) & 0x7f) == 4) ||
		    (((rate[i]) & 0x7f) == 11) || (((rate[i]) & 0x7f) == 22))
			return _TRUE;
		i++;
	}

	return _FALSE;
}

static uint rtw_is_cckratesonly_included(u8 *rate)
{
	u32 i = 0;

	while (rate[i] != 0) {
		if ((((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
		    (((rate[i]) & 0x7f) != 11) && (((rate[i]) & 0x7f) != 22))
			return _FALSE;
		i++;
	}

	return _TRUE;
}
#else
extern u8 WIFI_CCKRATES[];
extern u8 WIFI_OFDMRATES[];
extern uint rtw_is_cckrates_included(u8 *rate);
extern uint rtw_is_cckratesonly_included(u8 *rate);
#endif /* HOST_IEEE80211_REST_TEST */

int rtw_get_bit_value_from_ieee_value(u8 val)
{
	unsigned char dot11_rate_table[] = {
		2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108, 0
	};
	int i = 0;

	while (dot11_rate_table[i] != 0) {
		if (dot11_rate_table[i] == val)
			return BIT(i);
		i++;
	}
	return 0;
}

int rtw_check_network_type(unsigned char *rate, int ratelen, int channel)
{
	if (channel > 14) {
		if ((rtw_is_cckrates_included(rate)) == _TRUE)
			return WIRELESS_INVALID;
		else
			return WIRELESS_11A;
	} else {
		if ((rtw_is_cckratesonly_included(rate)) == _TRUE)
			return WIRELESS_11B;
		else if ((rtw_is_cckrates_included(rate)) == _TRUE)
			return WIRELESS_11BG;
		else
			return WIRELESS_11G;
	}
}

void rtw_set_supported_rate(u8 *SupportedRates, uint mode)
{
	_rtw_memset(SupportedRates, 0, NDIS_802_11_LENGTH_RATES_EX);

	switch (mode) {
	case WIRELESS_11B:
		_rtw_memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		break;

	case WIRELESS_11G:
	case WIRELESS_11A:
	case WIRELESS_11_5N:
	case WIRELESS_11A_5N:
	case WIRELESS_11_5AC:
		_rtw_memcpy(SupportedRates, WIFI_OFDMRATES,
			    IEEE80211_NUM_OFDM_RATESLEN);
		break;

	case WIRELESS_11BG:
	case WIRELESS_11G_24N:
	case WIRELESS_11_24N:
	case WIRELESS_11BG_24N:
		_rtw_memcpy(SupportedRates, WIFI_CCKRATES, IEEE80211_CCK_RATE_LEN);
		_rtw_memcpy(SupportedRates + IEEE80211_CCK_RATE_LEN, WIFI_OFDMRATES,
			    IEEE80211_NUM_OFDM_RATESLEN);
		break;
	}
}

void rtw_filter_suppport_rateie(WLAN_BSSID_EX *pbss_network, u8 keep)
{
	u8 i, idx = 0, new_rate[NDIS_802_11_LENGTH_RATES_EX], *p;
	uint iscck, isofdm, ie_orilen = 0, remain_len;
	u8 *remain_ies;

	p = rtw_get_ie(pbss_network->IEs + _BEACON_IE_OFFSET_, _SUPPORTEDRATES_IE_,
		       (sint *)&ie_orilen, pbss_network->IELength - _BEACON_IE_OFFSET_);
	if (!p)
		return;

	_rtw_memset(new_rate, 0, NDIS_802_11_LENGTH_RATES_EX);
	for (i = 0; i < ie_orilen; i++) {
		iscck = rtw_is_cck_rate(p[i + 2]);
		isofdm = rtw_is_ofdm_rate(p[i + 2]);
		if (((keep == CCK) && iscck) || ((keep == OFDM) && isofdm))
			new_rate[idx++] = rtw_is_basic_rate_ofdm(p[i + 2]) ?
						  p[i + 2] | IEEE80211_BASIC_RATE_MASK :
						  p[i + 2];
	}
	p[1] = idx;
	_rtw_memcpy(p + 2, new_rate, idx);
	remain_ies = p + 2 + ie_orilen;
	remain_len = pbss_network->IELength - (remain_ies - pbss_network->IEs);
	_rtw_memmove(p + 2 + idx, remain_ies, remain_len);
	pbss_network->IELength -= (ie_orilen - idx);
}

u8 rtw_update_rate_bymode(WLAN_BSSID_EX *pbss_network, u32 mode)
{
	u8 network_type, *p, *ie = pbss_network->IEs;
	sint ie_len;
	uint network_ielen = pbss_network->IELength;

	if (mode == WIRELESS_11B) {
		rtw_filter_suppport_rateie(pbss_network, CCK);
		p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _EXT_SUPPORTEDRATES_IE_,
			       &ie_len, pbss_network->IELength - _BEACON_IE_OFFSET_);
		if (p) {
			rtw_ies_remove_ie(ie, &network_ielen, _BEACON_IE_OFFSET_,
					  _EXT_SUPPORTEDRATES_IE_, NULL, 0);
			pbss_network->IELength -= ie_len;
		}
		network_type = WIRELESS_11B;
	} else {
		if (pbss_network->Configuration.DSConfig > 14) {
			rtw_filter_suppport_rateie(pbss_network, OFDM);
			network_type = WIRELESS_11A;
		} else {
			if ((mode & WIRELESS_11B) == 0) {
				rtw_filter_suppport_rateie(pbss_network, OFDM);
				network_type = WIRELESS_11G;
			} else {
				network_type = WIRELESS_11BG;
			}
		}
	}

	rtw_set_supported_rate(pbss_network->SupportedRates, network_type);

	return network_type;
}

int rtw_get_wpa_cipher_suite(u8 *s)
{
	if (_rtw_memcmp(s, WPA_CIPHER_SUITE_NONE, WPA_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_NONE;
	if (_rtw_memcmp(s, WPA_CIPHER_SUITE_WEP40, WPA_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_WEP40;
	if (_rtw_memcmp(s, WPA_CIPHER_SUITE_TKIP, WPA_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_TKIP;
	if (_rtw_memcmp(s, WPA_CIPHER_SUITE_CCMP, WPA_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_CCMP;
	if (_rtw_memcmp(s, WPA_CIPHER_SUITE_WEP104, WPA_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_WEP104;

	return 0;
}

int rtw_get_rsn_cipher_suite(u8 *s)
{
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_NONE, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_NONE;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_WEP40, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_WEP40;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_TKIP, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_TKIP;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_CCMP, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_CCMP;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_GCMP, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_GCMP;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_GCMP_256, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_GCMP_256;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_CCMP_256, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_CCMP_256;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_WEP104, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_WEP104;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_AES_128_CMAC, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_BIP_CMAC_128;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_BIP_GMAC_128, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_BIP_GMAC_128;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_BIP_GMAC_256, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_BIP_GMAC_256;
	if (_rtw_memcmp(s, RSN_CIPHER_SUITE_BIP_CMAC_256, RSN_SELECTOR_LEN) == _TRUE)
		return WPA_CIPHER_BIP_CMAC_256;
	return 0;
}

u32 rtw_get_akm_suite_bitmap(u8 *s)
{
	if (_rtw_memcmp(s, WLAN_AKM_8021X, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_8021X;
	if (_rtw_memcmp(s, WLAN_AKM_PSK, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_PSK;
	if (_rtw_memcmp(s, WLAN_AKM_FT_8021X, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FT_8021X;
	if (_rtw_memcmp(s, WLAN_AKM_FT_PSK, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FT_PSK;
	if (_rtw_memcmp(s, WLAN_AKM_8021X_SHA256, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_8021X_SHA256;
	if (_rtw_memcmp(s, WLAN_AKM_PSK_SHA256, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_PSK_SHA256;
	if (_rtw_memcmp(s, WLAN_AKM_TDLS, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_TDLS;
	if (_rtw_memcmp(s, WLAN_AKM_SAE, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_SAE;
	if (_rtw_memcmp(s, WLAN_AKM_FT_OVER_SAE, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FT_OVER_SAE;
	if (_rtw_memcmp(s, WLAN_AKM_8021X_SUITE_B, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_8021X_SUITE_B;
	if (_rtw_memcmp(s, WLAN_AKM_8021X_SUITE_B_192, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_8021X_SUITE_B_192;
	if (_rtw_memcmp(s, WLAN_AKM_FILS_SHA256, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FILS_SHA256;
	if (_rtw_memcmp(s, WLAN_AKM_FILS_SHA384, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FILS_SHA384;
	if (_rtw_memcmp(s, WLAN_AKM_FT_FILS_SHA256, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FT_FILS_SHA256;
	if (_rtw_memcmp(s, WLAN_AKM_FT_FILS_SHA384, RSN_SELECTOR_LEN) == _TRUE)
		return WLAN_AKM_TYPE_FT_FILS_SHA384;

	return 0;
}
#endif /* !CONFIG_RUST || HOST_IEEE80211_REST_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_IEEE80211_REST_TEST)
u32 *rtw_ieee80211_rest_bss_dsconfig(WLAN_BSSID_EX *bss)
{
	return &bss->Configuration.DSConfig;
}

u32 *rtw_ieee80211_rest_bss_ielength(WLAN_BSSID_EX *bss)
{
	return &bss->IELength;
}

u8 *rtw_ieee80211_rest_bss_ies(WLAN_BSSID_EX *bss)
{
	return bss->IEs;
}

u8 *rtw_ieee80211_rest_bss_supported_rates(WLAN_BSSID_EX *bss)
{
	return bss->SupportedRates;
}
#endif /* CONFIG_RUST && !HOST_IEEE80211_REST_TEST */
