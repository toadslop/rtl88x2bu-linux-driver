// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_bcn_ie.rs (W3-75 PR5). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_BCN_IE) && !defined(HOST_AP_BCN_IE_TEST)

WLAN_BSSID_EX *rtw_rust_ap_bcn_mlme_network(_adapter *adapter)
{
	return &adapter->mlmeextpriv.mlmext_info.network;
}

u32 *rtw_rust_ap_bcn_net_ie_len(WLAN_BSSID_EX *net)
{
	return &net->IELength;
}

u8 *rtw_rust_ap_bcn_net_ies(WLAN_BSSID_EX *net)
{
	return net->IEs;
}

u8 *rtw_rust_ap_bcn_tim_bitmap(_adapter *adapter)
{
	return adapter->stapriv.tim_bitmap;
}

u8 rtw_rust_ap_bcn_aid_bmp_len(_adapter *adapter)
{
	return adapter->stapriv.aid_bmp_len;
}

#endif /* CONFIG_RUST_AP_BCN_IE && !HOST_AP_BCN_IE_TEST */
