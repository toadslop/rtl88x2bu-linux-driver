/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _RTW_MLME_REST_C_

#ifdef HOST_MLME_TEST
#include "host_mlme_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_MLME_TEST)

void rtw_generate_random_ibss(u8 *pibss)
{
	*((u32 *)(&pibss[2])) = rtw_random32();
	pibss[0] = 0x02; /* in ad-hoc mode local bit must set to 1 */
	pibss[1] = 0x11;
	pibss[2] = 0x87;
}

u8 *rtw_get_capability_from_ie(u8 *ie)
{
	return ie + 8 + 2;
}

u16 rtw_get_capability(WLAN_BSSID_EX *bss)
{
	u16 val;

	_rtw_memcpy((u8 *)&val, rtw_get_capability_from_ie(bss->IEs), 2);

	return le16_to_cpu(val);
}

u8 *rtw_get_timestampe_from_ie(u8 *ie)
{
	return ie + 0;
}

u8 *rtw_get_beacon_interval_from_ie(u8 *ie)
{
	return ie + 8;
}

int rtw_is_same_ibss(_adapter *adapter, struct wlan_network *pnetwork)
{
	int ret = _TRUE;
	struct security_priv *psecuritypriv = &adapter->securitypriv;

	if ((psecuritypriv->dot11PrivacyAlgrthm != _NO_PRIVACY_) &&
	    (pnetwork->network.Privacy == 0))
		ret = _FALSE;
	else if ((psecuritypriv->dot11PrivacyAlgrthm == _NO_PRIVACY_) &&
		 (pnetwork->network.Privacy == 1))
		ret = _FALSE;
	else
		ret = _TRUE;

	return ret;
}

int is_same_ess(WLAN_BSSID_EX *a, WLAN_BSSID_EX *b)
{
	return (a->Ssid.SsidLength == b->Ssid.SsidLength)
	       &&  _rtw_memcmp(a->Ssid.Ssid, b->Ssid.Ssid, a->Ssid.SsidLength) == _TRUE;
}

int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, u8 feature)
{
	u16 s_cap, d_cap;

	if (rtw_bug_check(dst, src, &s_cap, &d_cap) == _FALSE)
		return _FALSE;

	_rtw_memcpy((u8 *)&s_cap, rtw_get_capability_from_ie(src->IEs), 2);
	_rtw_memcpy((u8 *)&d_cap, rtw_get_capability_from_ie(dst->IEs), 2);

	s_cap = le16_to_cpu(s_cap);
	d_cap = le16_to_cpu(d_cap);

#ifdef CONFIG_P2P
	if ((feature == 1) &&
	    (_rtw_memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN) == _TRUE))
		return _TRUE;
#endif

	if (((_rtw_memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN)) == _TRUE) &&
	    ((s_cap & WLAN_CAPABILITY_IBSS) == (d_cap & WLAN_CAPABILITY_IBSS)) &&
	    ((s_cap & WLAN_CAPABILITY_BSS) == (d_cap & WLAN_CAPABILITY_BSS))) {
		if ((src->Ssid.SsidLength == dst->Ssid.SsidLength) &&
		    (((_rtw_memcmp(src->Ssid.Ssid, dst->Ssid.Ssid, src->Ssid.SsidLength)) == _TRUE) ||
		     (is_all_null((char *)src->Ssid.Ssid, src->Ssid.SsidLength) == _TRUE ||
		      is_all_null((char *)dst->Ssid.Ssid, dst->Ssid.SsidLength) == _TRUE)))
			return _TRUE;
		else if ((src->Ssid.SsidLength == 0 || dst->Ssid.SsidLength == 0))
			return _TRUE;
		else
			return _FALSE;
	} else {
		return _FALSE;
	}
}

#endif /* !CONFIG_RUST || HOST_MLME_TEST */
