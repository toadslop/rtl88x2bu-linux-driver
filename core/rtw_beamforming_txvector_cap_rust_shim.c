// SPDX-License-Identifier: GPL-2.0
/* W3-201: C field accessors for rust/rtw_beamforming.rs txvector + cap leaf (v2). */

#include <drv_types.h>
#include <hal_data.h>

#if defined(CONFIG_RUST) && defined(CONFIG_BEAMFORMING) && defined(RTW_BEAMFORMING_VERSION_2)

u32 rtw_bf_rust_txvec_fwstate(_adapter *adapter)
{
	return adapter->mlmepriv.fw_state;
}

void rtw_bf_rust_txvec_ap_mac(_adapter *adapter, u8 *out)
{
	u8 *mac = adapter_mac_addr(adapter);

	if (out)
		_rtw_memcpy(out, mac, ETH_ALEN);
}

u16 rtw_bf_rust_txvec_mlme_ht(_adapter *adapter)
{
	return adapter->mlmepriv.htpriv.beamform_cap;
}

u16 rtw_bf_rust_txvec_mlme_vht(_adapter *adapter)
{
#ifdef CONFIG_80211AC_VHT
	return adapter->mlmepriv.vhtpriv.beamform_cap;
#else
	return 0;
#endif
}

u16 rtw_bf_rust_txvec_sta_aid(struct sta_info *sta)
{
	return sta->cmn.aid;
}

void rtw_bf_rust_txvec_sta_mac(struct sta_info *sta, u8 *out)
{
	if (out)
		_rtw_memcpy(out, sta->cmn.mac_addr, ETH_ALEN);
}

u8 rtw_bf_rust_txvec_sta_wm(struct sta_info *sta)
{
	return sta->wireless_mode;
}

u16 rtw_bf_rust_txvec_sta_ht(struct sta_info *sta)
{
	return sta->htpriv.beamform_cap;
}

u16 rtw_bf_rust_txvec_sta_vht(struct sta_info *sta)
{
#ifdef CONFIG_80211AC_VHT
	return sta->vhtpriv.beamform_cap;
#else
	return 0;
#endif
}

u8 *rtw_bf_rust_txvec_info_cap(_adapter *adapter)
{
	struct beamforming_info *info = GET_BEAMFORM_INFO(adapter);

	return (u8 *)&info->beamforming_cap;
}

#endif
