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

#ifdef HOST_IEEE80211_REST_TEST
extern u8 RTW_WPA_OUI_TYPE[];
#endif

int rtw_parse_wpa_ie(u8 *wpa_ie, int wpa_ie_len, int *group_cipher,
		     int *pairwise_cipher, u32 *akm)
{
	int i, ret = _SUCCESS;
	int left, count;
	u8 *pos;
	u8 SUITE_1X[4] = {0x00, 0x50, 0xf2, 1};

	if (wpa_ie_len <= 0)
		return _FAIL;

	if ((*wpa_ie != _WPA_IE_ID_) || (*(wpa_ie + 1) != (u8)(wpa_ie_len - 2)) ||
	    (_rtw_memcmp(wpa_ie + 2, RTW_WPA_OUI_TYPE, WPA_SELECTOR_LEN) != _TRUE))
		return _FAIL;

	pos = wpa_ie;
	pos += 8;
	left = wpa_ie_len - 8;

	if (left >= WPA_SELECTOR_LEN) {
		*group_cipher = rtw_get_wpa_cipher_suite(pos);
		pos += WPA_SELECTOR_LEN;
		left -= WPA_SELECTOR_LEN;
	} else if (left > 0) {
		return _FAIL;
	}

	if (left >= 2) {
		count = RTW_GET_LE16(pos);
		pos += 2;
		left -= 2;

		if (count == 0 || left < count * WPA_SELECTOR_LEN)
			return _FAIL;

		for (i = 0; i < count; i++) {
			*pairwise_cipher |= rtw_get_wpa_cipher_suite(pos);
			pos += WPA_SELECTOR_LEN;
			left -= WPA_SELECTOR_LEN;
		}
	} else if (left == 1) {
		return _FAIL;
	}

	if (akm) {
		if (left >= 6) {
			pos += 2;
			if (_rtw_memcmp(pos, SUITE_1X, 4) == 1)
				*akm = WLAN_AKM_TYPE_8021X;
		}
	}

	return ret;
}

int rtw_rsne_info_parse(const u8 *ie, uint ie_len, struct rsne_info *info)
{
	const u8 *pos = ie;
	u16 ver;
	u16 cnt;

	_rtw_memset(info, 0, sizeof(struct rsne_info));

	if (ie + ie_len < pos + 4)
		goto err;

	if (*ie != WLAN_EID_RSN || *(ie + 1) != ie_len - 2)
		goto err;
	pos += 2;

	ver = RTW_GET_LE16(pos);
	if (1 != ver)
		goto err;
	pos += 2;

	if (ie + ie_len < pos + 4) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	info->gcs = (u8 *)pos;
	pos += 4;

	if (ie + ie_len < pos + 2) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	cnt = RTW_GET_LE16(pos);
	pos += 2;
	if (ie + ie_len < pos + 4 * cnt) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	info->pcs_cnt = cnt;
	info->pcs_list = (u8 *)pos;
	pos += 4 * cnt;

	if (ie + ie_len < pos + 2) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	cnt = RTW_GET_LE16(pos);
	pos += 2;
	if (ie + ie_len < pos + 4 * cnt) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	info->akm_cnt = cnt;
	info->akm_list = (u8 *)pos;
	pos += 4 * cnt;

	if (ie + ie_len < pos + 2) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	info->cap = (u8 *)pos;
	pos += 2;

	if (ie + ie_len < pos + 2) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	cnt = RTW_GET_LE16(pos);
	pos += 2;
	if (ie + ie_len < pos + 16 * cnt)
		goto err;
	info->pmkid_cnt = cnt;
	info->pmkid_list = (u8 *)pos;
	pos += 16 * cnt;

	if (ie + ie_len < pos + 4) {
		if (ie + ie_len != pos)
			goto err;
		goto exit;
	}
	info->gmcs = (u8 *)pos;

exit:
	return _SUCCESS;

err:
	info->err = 1;
	return _FAIL;
}

int rtw_parse_wpa2_ie(u8 *rsn_ie, int rsn_ie_len, int *group_cipher,
		      int *pairwise_cipher, int *gmcs, u32 *akm, u8 *mfp_opt,
		      u8 *spp_opt)
{
	struct rsne_info info;
	int i, ret = _SUCCESS;

	ret = rtw_rsne_info_parse(rsn_ie, rsn_ie_len, &info);
	if (ret != _SUCCESS)
		goto exit;

	if (group_cipher) {
		if (info.gcs)
			*group_cipher = rtw_get_rsn_cipher_suite(info.gcs);
		else
			*group_cipher = 0;
	}

	if (pairwise_cipher) {
		*pairwise_cipher = 0;
		if (info.pcs_list) {
			for (i = 0; i < info.pcs_cnt; i++)
				*pairwise_cipher |= rtw_get_rsn_cipher_suite(
					info.pcs_list + 4 * i);
		}
	}

	if (gmcs) {
		if (info.gmcs)
			*gmcs = rtw_get_rsn_cipher_suite(info.gmcs);
		else
			*gmcs = WPA_CIPHER_BIP_CMAC_128;
	}

	if (akm) {
		*akm = 0;
		if (info.akm_list) {
			for (i = 0; i < info.akm_cnt; i++)
				*akm |= rtw_get_akm_suite_bitmap(info.akm_list +
								 4 * i);
		}
	}

	if (mfp_opt) {
		*mfp_opt = MFP_NO;
		if (info.cap)
			*mfp_opt = GET_RSN_CAP_MFP_OPTION(info.cap);
	}

	if (spp_opt) {
		*spp_opt = 0;
		if (info.cap)
			*spp_opt = GET_RSN_CAP_SPP_OPT(info.cap);
	}

exit:
	return ret;
}

/*
 * W3-29: WAPI/WPS/sec-IE getter helpers extracted from core/rtw_ieee80211.c.
 */

/* #ifdef CONFIG_WAPI_SUPPORT */
int rtw_get_wapi_ie(u8 *in_ie, uint in_len, u8 *wapi_ie, u16 *wapi_len)
{
	int len = 0;
	u8 authmode;
	uint cnt;
	u8 wapi_oui1[4] = {0x0, 0x14, 0x72, 0x01};
	u8 wapi_oui2[4] = {0x0, 0x14, 0x72, 0x02};

	if (wapi_len)
		*wapi_len = 0;

	if (!in_ie || in_len <= 0)
		return len;

	cnt = (_TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_);

	while (cnt < in_len) {
		authmode = in_ie[cnt];

		if (authmode == _WAPI_IE_ &&
		    (_rtw_memcmp(&in_ie[cnt + 6], wapi_oui1, 4) == _TRUE ||
		     _rtw_memcmp(&in_ie[cnt + 6], wapi_oui2, 4) == _TRUE)) {
			if (wapi_ie)
				_rtw_memcpy(wapi_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (wapi_len)
				*wapi_len = in_ie[cnt + 1] + 2;

			cnt += in_ie[cnt + 1] + 2;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}
	}

	if (wapi_len)
		len = *wapi_len;

	return len;
}
/* #endif */

int rtw_get_sec_ie(u8 *in_ie, uint in_len, u8 *rsn_ie, u16 *rsn_len,
		   u8 *wpa_ie, u16 *wpa_len)
{
	u8 authmode;
	u8 wpa_oui[4] = {0x0, 0x50, 0xf2, 0x01};
	uint cnt;

	cnt = (_TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_);

	while (cnt < in_len) {
		authmode = in_ie[cnt];

		if ((authmode == _WPA_IE_ID_) &&
		    (_rtw_memcmp(&in_ie[cnt + 2], &wpa_oui[0], 4) == _TRUE)) {
			if (wpa_ie)
				_rtw_memcpy(wpa_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			*wpa_len = in_ie[cnt + 1] + 2;
			cnt += in_ie[cnt + 1] + 2;
		} else {
			if (authmode == _WPA2_IE_ID_) {
				if (rsn_ie)
					_rtw_memcpy(rsn_ie, &in_ie[cnt],
						    in_ie[cnt + 1] + 2);

				*rsn_len = in_ie[cnt + 1] + 2;
				cnt += in_ie[cnt + 1] + 2;
			} else {
				cnt += in_ie[cnt + 1] + 2;
			}
		}
	}

	return *rsn_len + *wpa_len;
}

u8 rtw_is_wps_ie(u8 *ie_ptr, uint *wps_ielen)
{
	u8 match = _FALSE;
	u8 eid, wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

	if (ie_ptr == NULL)
		return match;

	eid = ie_ptr[0];

	if ((eid == _WPA_IE_ID_) &&
	    (_rtw_memcmp(&ie_ptr[2], wps_oui, 4) == _TRUE)) {
		*wps_ielen = ie_ptr[1] + 2;
		match = _TRUE;
	}
	return match;
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
