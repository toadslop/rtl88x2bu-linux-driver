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
#endif

extern u8 WIFI_CCKRATES[];
extern u8 WIFI_OFDMRATES[];

uint rtw_is_cckrates_included(u8 *rate)
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

uint rtw_is_cckratesonly_included(u8 *rate)
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

/*
 * W3-30: string/MAC address helpers extracted from core/rtw_ieee80211.c.
 */

static u8 key_char2num(u8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else
		return 0xff;
}

u8 str_2char2num(u8 hch, u8 lch)
{
	return (key_char2num(hch) * 10) + key_char2num(lch);
}

u8 key_2char2num(u8 hch, u8 lch)
{
	return (key_char2num(hch) << 4) | key_char2num(lch);
}

void macstr2num(u8 *dst, u8 *src)
{
	int jj, kk;

	for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3)
		dst[jj] = key_2char2num(src[kk], src[kk + 1]);
}

u8 convert_ip_addr(u8 hch, u8 mch, u8 lch)
{
	return (key_char2num(hch) * 100) + (key_char2num(mch) * 10) +
	       key_char2num(lch);
}

u8 rtw_check_invalid_mac_address(u8 *mac_addr, u8 check_local_bit)
{
	u8 null_mac_addr[ETH_ALEN] = {0, 0, 0, 0, 0, 0};
	u8 multi_mac_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 res = _FALSE;

	if (_rtw_memcmp(mac_addr, null_mac_addr, ETH_ALEN)) {
		res = _TRUE;
		goto func_exit;
	}

	if (_rtw_memcmp(mac_addr, multi_mac_addr, ETH_ALEN)) {
		res = _TRUE;
		goto func_exit;
	}

	if (mac_addr[0] & BIT0) {
		res = _TRUE;
		goto func_exit;
	}

	if (check_local_bit == _TRUE) {
		if (mac_addr[0] & BIT1) {
			res = _TRUE;
			goto func_exit;
		}
	}

func_exit:
	return res;
}

extern char *rtw_initmac;

void rtw_macaddr_cfg(u8 *out, const u8 *hw_mac_addr)
{
#define DEFAULT_RANDOM_MACADDR 1
	u8 mac[ETH_ALEN];

	if (out == NULL) {
		rtw_warn_on(1);
		return;
	}

	if (rtw_initmac) {
		int jj, kk;

		for (jj = 0, kk = 0; jj < ETH_ALEN; jj++, kk += 3)
			mac[jj] = key_2char2num(rtw_initmac[kk], rtw_initmac[kk + 1]);

		goto err_chk;
	}

	if (hw_mac_addr) {
		_rtw_memcpy(mac, hw_mac_addr, ETH_ALEN);
		goto err_chk;
	}

err_chk:
	if (rtw_check_invalid_mac_address(mac, _TRUE) == _TRUE) {
#if DEFAULT_RANDOM_MACADDR
		RTW_ERR("invalid mac addr:" MAC_FMT ", assign random MAC\n",
			MAC_ARG(mac));
		*((u32 *)(&mac[2])) = rtw_random32();
		mac[0] = 0x00;
		mac[1] = 0xe0;
		mac[2] = 0x4c;
#else
		RTW_ERR("invalid mac addr:" MAC_FMT ", assign default one\n",
			MAC_ARG(mac));
		mac[0] = 0x00;
		mac[1] = 0xe0;
		mac[2] = 0x4c;
		mac[3] = 0x87;
		mac[4] = 0x00;
		mac[5] = 0x00;
#endif
	}

	_rtw_memcpy(out, mac, ETH_ALEN);
	RTW_INFO("%s mac addr:" MAC_FMT "\n", __func__, MAC_ARG(out));
}

#if defined(HOST_IEEE80211_REST_CHBW_TEST) || !defined(HOST_IEEE80211_REST_TEST)
/*
 * W3-31: channel/bandwidth grouping helpers extracted from core/rtw_ieee80211.c.
 */

void rtw_ies_get_chbw(u8 *ies, int ies_len, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht)
{
	u8 *p;
	int ie_len;

	*ch = 0;
	*bw = CHANNEL_WIDTH_20;
	*offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

	p = rtw_get_ie(ies, _DSSET_IE_, &ie_len, ies_len);
	if (p && ie_len > 0)
		*ch = *(p + 2);

#ifdef CONFIG_80211N_HT
	if (ht || vht) {
		u8 *ht_cap_ie, *ht_op_ie;
		int ht_cap_ielen, ht_op_ielen;

		ht_cap_ie = rtw_get_ie(ies, EID_HTCapability, &ht_cap_ielen,
				       ies_len);
		if (ht_cap_ie && ht_cap_ielen) {
			if (GET_HT_CAP_ELE_CHL_WIDTH(ht_cap_ie + 2))
				*bw = CHANNEL_WIDTH_40;
		}

		ht_op_ie = rtw_get_ie(ies, EID_HTInfo, &ht_op_ielen, ies_len);
		if (ht_op_ie && ht_op_ielen) {
			if (*ch == 0)
				*ch = GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2);
			else if (*ch != 0 &&
				 *ch != GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2)) {
				RTW_INFO("%s ch inconsistent, DSSS:%u, HT primary:%u\n",
					 __func__, *ch,
					 GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2));
			}

			if (!GET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2))
				*bw = CHANNEL_WIDTH_20;

			if (*bw == CHANNEL_WIDTH_40) {
				switch (GET_HT_OP_ELE_2ND_CHL_OFFSET(
						ht_op_ie + 2)) {
				case SCA:
					*offset = HAL_PRIME_CHNL_OFFSET_LOWER;
					break;
				case SCB:
					*offset = HAL_PRIME_CHNL_OFFSET_UPPER;
					break;
				}
			}
		}

#ifdef CONFIG_80211AC_VHT
		if (vht) {
			u8 *vht_op_ie;
			int vht_op_ielen;

			vht_op_ie = rtw_get_ie(ies, EID_VHTOperation,
					       &vht_op_ielen, ies_len);
			if (vht_op_ie && vht_op_ielen) {
				if (GET_VHT_OPERATION_ELE_CHL_WIDTH(
					    vht_op_ie + 2) >= 1)
					*bw = CHANNEL_WIDTH_80;
			}
		}
#endif /* CONFIG_80211AC_VHT */
	}
#endif /* CONFIG_80211N_HT */
}

void rtw_bss_get_chbw(WLAN_BSSID_EX *bss, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht)
{
	rtw_ies_get_chbw(bss->IEs + sizeof(NDIS_802_11_FIXED_IEs),
			 bss->IELength - sizeof(NDIS_802_11_FIXED_IEs), ch, bw,
			 offset, ht, vht);

	if (*ch == 0)
		*ch = bss->Configuration.DSConfig;
	else if (*ch != bss->Configuration.DSConfig) {
		RTW_INFO("inconsistent ch - ies:%u bss->Configuration.DSConfig:%u\n",
			 *ch, bss->Configuration.DSConfig);
		*ch = bss->Configuration.DSConfig;
		rtw_warn_on(1);
	}
}

bool rtw_is_chbw_grouped(u8 ch_a, u8 bw_a, u8 offset_a, u8 ch_b, u8 bw_b,
			 u8 offset_b)
{
	bool is_grouped = _FALSE;

	if (ch_a != ch_b)
		goto exit;
	else if ((bw_a == CHANNEL_WIDTH_40 || bw_a == CHANNEL_WIDTH_80) &&
		 (bw_b == CHANNEL_WIDTH_40 || bw_b == CHANNEL_WIDTH_80)) {
		if (offset_a != offset_b)
			goto exit;
	}

	is_grouped = _TRUE;

exit:
	return is_grouped;
}

void rtw_sync_chbw(u8 *req_ch, u8 *req_bw, u8 *req_offset, u8 *g_ch, u8 *g_bw,
		   u8 *g_offset)
{
	*req_ch = *g_ch;

	if (*req_bw == CHANNEL_WIDTH_80 && *g_ch <= 14)
		*req_bw = CHANNEL_WIDTH_40;

	switch (*req_bw) {
	case CHANNEL_WIDTH_80:
		if (*g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80)
			*req_offset = *g_offset;
		else if (*g_bw == CHANNEL_WIDTH_20)
			rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);

		if (*req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
			RTW_ERR("%s req 80MHz BW without offset, down to 20MHz\n",
				__func__);
			rtw_warn_on(1);
			*req_bw = CHANNEL_WIDTH_20;
		}
		break;
	case CHANNEL_WIDTH_40:
		if (*g_bw == CHANNEL_WIDTH_40 || *g_bw == CHANNEL_WIDTH_80)
			*req_offset = *g_offset;
		else if (*g_bw == CHANNEL_WIDTH_20)
			rtw_get_offset_by_chbw(*req_ch, *req_bw, req_offset);

		if (*req_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE) {
			RTW_ERR("%s req 40MHz BW without offset, down to 20MHz\n",
				__func__);
			rtw_warn_on(1);
			*req_bw = CHANNEL_WIDTH_20;
		}
		break;
	case CHANNEL_WIDTH_20:
		*req_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	default:
		RTW_ERR("%s req unsupported BW:%u\n", __func__, *req_bw);
		rtw_warn_on(1);
	}

	if (*req_bw > *g_bw) {
		*g_bw = *req_bw;
		*g_offset = *req_offset;
	}
}
#endif /* HOST_IEEE80211_REST_CHBW_TEST || !HOST_IEEE80211_REST_TEST */

#if defined(HOST_IEEE80211_REST_FRAME_HT_TEST) || !defined(HOST_IEEE80211_REST_TEST)
/*
 * W3-32: frame header and HT MCS helpers extracted from core/rtw_ieee80211.c.
 */

#ifdef HOST_IEEE80211_REST_TEST
static u8 rtw_ht_mcsset_to_nss_local(u8 *supp_mcs_set)
{
	u8 nss = 1;

	if (supp_mcs_set[3])
		nss = 4;
	else if (supp_mcs_set[2])
		nss = 3;
	else if (supp_mcs_set[1])
		nss = 2;
	else if (supp_mcs_set[0])
		nss = 1;
	return nss;
}
#else
extern u8 rtw_ht_mcsset_to_nss(u8 *supp_mcs_set);
#define rtw_ht_mcsset_to_nss_local rtw_ht_mcsset_to_nss
#endif /* HOST_IEEE80211_REST_TEST */

int ieee80211_is_empty_essid(const char *essid, int essid_len)
{
	if (essid_len == 1 && essid[0] == ' ')
		return 1;

	while (essid_len) {
		essid_len--;
		if (essid[essid_len] != '\0')
			return 0;
	}

	return 1;
}

int ieee80211_get_hdrlen(u16 fc)
{
	int hdrlen = 24;

	switch (WLAN_FC_GET_TYPE(fc)) {
	case RTW_IEEE80211_FTYPE_DATA:
		if (fc & RTW_IEEE80211_STYPE_QOS_DATA)
			hdrlen += 2;
		if ((fc & RTW_IEEE80211_FCTL_FROMDS) && (fc & RTW_IEEE80211_FCTL_TODS))
			hdrlen += 6;
		break;
	case RTW_IEEE80211_FTYPE_CTL:
		switch (WLAN_FC_GET_STYPE(fc)) {
		case RTW_IEEE80211_STYPE_CTS:
		case RTW_IEEE80211_STYPE_ACK:
			hdrlen = 10;
			break;
		default:
			hdrlen = 16;
			break;
		}
		break;
	}

	return hdrlen;
}

u16 rtw_ht_mcs_rate(u8 bw_40MHz, u8 short_GI, unsigned char *MCS_rate)
{
	u16 max_rate = 0;

	if (MCS_rate[3]) {
		if (MCS_rate[3] & BIT(7))
			max_rate = (bw_40MHz) ? ((short_GI) ? 6000 : 5400) : ((short_GI) ? 2889 : 2600);
		else if (MCS_rate[3] & BIT(6))
			max_rate = (bw_40MHz) ? ((short_GI) ? 5400 : 4860) : ((short_GI) ? 2600 : 2340);
		else if (MCS_rate[3] & BIT(5))
			max_rate = (bw_40MHz) ? ((short_GI) ? 4800 : 4320) : ((short_GI) ? 2311 : 2080);
		else if (MCS_rate[3] & BIT(4))
			max_rate = (bw_40MHz) ? ((short_GI) ? 3600 : 3240) : ((short_GI) ? 1733 : 1560);
		else if (MCS_rate[3] & BIT(3))
			max_rate = (bw_40MHz) ? ((short_GI) ? 2400 : 2160) : ((short_GI) ? 1156 : 1040);
		else if (MCS_rate[3] & BIT(2))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1800 : 1620) : ((short_GI) ? 867 : 780);
		else if (MCS_rate[3] & BIT(1))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1200 : 1080) : ((short_GI) ? 578 : 520);
		else if (MCS_rate[3] & BIT(0))
			max_rate = (bw_40MHz) ? ((short_GI) ? 600 : 540) : ((short_GI) ? 289 : 260);
	} else if (MCS_rate[2]) {
		if (MCS_rate[2] & BIT(7))
			max_rate = (bw_40MHz) ? ((short_GI) ? 4500 : 4050) : ((short_GI) ? 2167 : 1950);
		else if (MCS_rate[2] & BIT(6))
			max_rate = (bw_40MHz) ? ((short_GI) ? 4050 : 3645) : ((short_GI) ? 1950 : 1750);
		else if (MCS_rate[2] & BIT(5))
			max_rate = (bw_40MHz) ? ((short_GI) ? 3600 : 3240) : ((short_GI) ? 1733 : 1560);
		else if (MCS_rate[2] & BIT(4))
			max_rate = (bw_40MHz) ? ((short_GI) ? 2700 : 2430) : ((short_GI) ? 1300 : 1170);
		else if (MCS_rate[2] & BIT(3))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1800 : 1620) : ((short_GI) ? 867 : 780);
		else if (MCS_rate[2] & BIT(2))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1350 : 1215) : ((short_GI) ? 650 : 585);
		else if (MCS_rate[2] & BIT(1))
			max_rate = (bw_40MHz) ? ((short_GI) ? 900 : 810) : ((short_GI) ? 433 : 390);
		else if (MCS_rate[2] & BIT(0))
			max_rate = (bw_40MHz) ? ((short_GI) ? 450 : 405) : ((short_GI) ? 217 : 195);
	} else if (MCS_rate[1]) {
		if (MCS_rate[1] & BIT(7))
			max_rate = (bw_40MHz) ? ((short_GI) ? 3000 : 2700) : ((short_GI) ? 1444 : 1300);
		else if (MCS_rate[1] & BIT(6))
			max_rate = (bw_40MHz) ? ((short_GI) ? 2700 : 2430) : ((short_GI) ? 1300 : 1170);
		else if (MCS_rate[1] & BIT(5))
			max_rate = (bw_40MHz) ? ((short_GI) ? 2400 : 2160) : ((short_GI) ? 1156 : 1040);
		else if (MCS_rate[1] & BIT(4))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1800 : 1620) : ((short_GI) ? 867 : 780);
		else if (MCS_rate[1] & BIT(3))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1200 : 1080) : ((short_GI) ? 578 : 520);
		else if (MCS_rate[1] & BIT(2))
			max_rate = (bw_40MHz) ? ((short_GI) ? 900 : 810) : ((short_GI) ? 433 : 390);
		else if (MCS_rate[1] & BIT(1))
			max_rate = (bw_40MHz) ? ((short_GI) ? 600 : 540) : ((short_GI) ? 289 : 260);
		else if (MCS_rate[1] & BIT(0))
			max_rate = (bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 : 130);
	} else {
		if (MCS_rate[0] & BIT(7))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1500 : 1350) : ((short_GI) ? 722 : 650);
		else if (MCS_rate[0] & BIT(6))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1350 : 1215) : ((short_GI) ? 650 : 585);
		else if (MCS_rate[0] & BIT(5))
			max_rate = (bw_40MHz) ? ((short_GI) ? 1200 : 1080) : ((short_GI) ? 578 : 520);
		else if (MCS_rate[0] & BIT(4))
			max_rate = (bw_40MHz) ? ((short_GI) ? 900 : 810) : ((short_GI) ? 433 : 390);
		else if (MCS_rate[0] & BIT(3))
			max_rate = (bw_40MHz) ? ((short_GI) ? 600 : 540) : ((short_GI) ? 289 : 260);
		else if (MCS_rate[0] & BIT(2))
			max_rate = (bw_40MHz) ? ((short_GI) ? 450 : 405) : ((short_GI) ? 217 : 195);
		else if (MCS_rate[0] & BIT(1))
			max_rate = (bw_40MHz) ? ((short_GI) ? 300 : 270) : ((short_GI) ? 144 : 130);
		else if (MCS_rate[0] & BIT(0))
			max_rate = (bw_40MHz) ? ((short_GI) ? 150 : 135) : ((short_GI) ? 72 : 65);
	}

	return max_rate;
}

u8 rtw_ht_cap_get_rx_nss(u8 *ht_cap)
{
	u8 *ht_mcs_set = HT_CAP_ELE_SUP_MCS_SET(ht_cap);

	return rtw_ht_mcsset_to_nss_local(ht_mcs_set);
}

u8 rtw_ht_cap_get_tx_nss(u8 *ht_cap)
{
	if (GET_HT_CAP_ELE_TX_MCS_DEF(ht_cap) && GET_HT_CAP_ELE_TRX_MCS_NEQ(ht_cap))
		return GET_HT_CAP_ELE_TX_MAX_SS(ht_cap) + 1;

	return rtw_ht_cap_get_rx_nss(ht_cap);
}

int rtw_action_frame_parse(const u8 *frame, u32 frame_len, u8 *category, u8 *action)
{
	const u8 *frame_body = frame + sizeof(struct rtw_ieee80211_hdr_3addr);
	u16 fc;
	u8 c;
	u8 a = ACT_PUBLIC_MAX;

	(void)frame_len;

	fc = le16_to_cpu(((struct rtw_ieee80211_hdr_3addr *)frame)->frame_ctl);

	if ((fc & (RTW_IEEE80211_FCTL_FTYPE | RTW_IEEE80211_FCTL_STYPE))
	    != (RTW_IEEE80211_FTYPE_MGMT | RTW_IEEE80211_STYPE_ACTION)
	   )
		return _FALSE;

	c = frame_body[0];

	switch (c) {
	case RTW_WLAN_CATEGORY_P2P:
		break;
	default:
		a = frame_body[1];
	}

	if (category)
		*category = c;
	if (action)
		*action = a;

	return _TRUE;
}
#endif /* HOST_IEEE80211_REST_FRAME_HT_TEST || !HOST_IEEE80211_REST_TEST */
#endif /* !CONFIG_RUST || HOST_IEEE80211_REST_TEST */

#if !defined(CONFIG_RUST) || defined(HOST_IEEE80211_REST_RATE_SECTION_TEST) || \
	!defined(HOST_IEEE80211_REST_TEST)
/*
 * W3-41: rate-section and channel-offset mapping helpers extracted from
 * core/rtw_ieee80211.c. Rust port replaces kernel build in PR3.
 */
RATE_SECTION mgn_rate_to_rs(enum MGN_RATE rate)
{
	RATE_SECTION rs = RATE_SECTION_NUM;

	if (IS_CCK_RATE(rate))
		rs = CCK;
	else if (IS_OFDM_RATE(rate))
		rs = OFDM;
	else if (IS_HT1SS_RATE(rate))
		rs = HT_1SS;
	else if (IS_HT2SS_RATE(rate))
		rs = HT_2SS;
	else if (IS_HT3SS_RATE(rate))
		rs = HT_3SS;
	else if (IS_HT4SS_RATE(rate))
		rs = HT_4SS;
	else if (IS_VHT1SS_RATE(rate))
		rs = VHT_1SS;
	else if (IS_VHT2SS_RATE(rate))
		rs = VHT_2SS;
	else if (IS_VHT3SS_RATE(rate))
		rs = VHT_3SS;
	else if (IS_VHT4SS_RATE(rate))
		rs = VHT_4SS;

	return rs;
}

uint rtw_get_cckrate_size(u8 *rate, u32 rate_length)
{
	int i = 0;

	while (i < rate_length) {
		RTW_DBG("%s, rate[%d]=%u\n", __FUNCTION__, i, rate[i]);
		if (((rate[i] & 0x7f) == 2) || ((rate[i] & 0x7f) == 4) ||
		    ((rate[i] & 0x7f) == 11) || ((rate[i] & 0x7f) == 22))
			i++;
		else
			break;
	}
	return i;
}

uint rtw_get_rateset_len(u8 *rateset)
{
	uint i = 0;

	while (1) {
		if ((rateset[i]) == 0)
			break;

		if (i > 12)
			break;

		i++;
	}
	return i;
}

u8 secondary_ch_offset_to_hal_ch_offset(u8 ch_offset)
{
	if (ch_offset == SCN)
		return HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	else if (ch_offset == SCA)
		return HAL_PRIME_CHNL_OFFSET_LOWER;
	else if (ch_offset == SCB)
		return HAL_PRIME_CHNL_OFFSET_UPPER;

	return HAL_PRIME_CHNL_OFFSET_DONT_CARE;
}

u8 hal_ch_offset_to_secondary_ch_offset(u8 ch_offset)
{
	if (ch_offset == HAL_PRIME_CHNL_OFFSET_DONT_CARE)
		return SCN;
	else if (ch_offset == HAL_PRIME_CHNL_OFFSET_LOWER)
		return SCA;
	else if (ch_offset == HAL_PRIME_CHNL_OFFSET_UPPER)
		return SCB;

	return SCN;
}
#endif /* !CONFIG_RUST || HOST_IEEE80211_REST_RATE_SECTION_TEST || kernel */

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
