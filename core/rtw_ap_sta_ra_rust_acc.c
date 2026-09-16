// SPDX-License-Identifier: GPL-2.0
/* Kernel accessors for rust/rtw_ap_sta_ra.rs (W3-83 PR3). */
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_STA_RA) && !defined(HOST_AP_STA_RA_TEST)

u32 rtw_rust_ap_sta_ra_ds_config(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	WLAN_BSSID_EX *pcur_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;

	return pcur_network->Configuration.DSConfig;
}

u32 rtw_rust_ap_sta_ra_state(struct sta_info *psta)
{
	return psta->state;
}

u64 rtw_rust_ap_sta_ra_ramask(struct sta_info *psta)
{
	return psta->cmn.ra_info.ramask;
}

u8 rtw_rust_ap_sta_ra_vht_option(struct sta_info *psta)
{
	return psta->vhtpriv.vht_option;
}

void rtw_rust_ap_sta_ra_set_wireless_mode(struct sta_info *psta, u8 mode)
{
	psta->wireless_mode = mode;
}

#endif /* CONFIG_RUST_AP_STA_RA && !HOST_AP_STA_RA_TEST */
