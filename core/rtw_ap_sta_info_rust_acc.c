// SPDX-License-Identifier: GPL-2.0
#include <drv_types.h>

#if defined(CONFIG_RUST_AP_STA_INFO) && !defined(HOST_AP_STA_INFO_TEST)

u8 rtw_rust_ap_sta_info_ap_bf_cap(_adapter *padapter)
{
	return padapter->mlmepriv.htpriv.beamform_cap;
}

const u8 *rtw_rust_ap_sta_info_sta_ht_cap(struct sta_info *psta)
{
	return (const u8 *)&psta->htpriv.ht_cap;
}

void rtw_rust_ap_sta_info_set_sta_bf_cap(struct sta_info *psta, u8 cap)
{
	psta->htpriv.beamform_cap = cap;
}

void rtw_rust_ap_sta_info_set_ht_beamform_cap(struct sta_info *psta, u8 cap)
{
	psta->cmn.bf_info.ht_beamform_cap = cap;
}

u8 rtw_rust_ap_sta_info_get_ampdu_para(_adapter *padapter)
{
	return padapter->mlmeextpriv.mlmext_info.HT_caps.u.HT_cap_element.AMPDU_para;
}

u16 rtw_rust_ap_sta_info_get_ht_caps_info(_adapter *padapter)
{
	return padapter->mlmeextpriv.mlmext_info.HT_caps.u.HT_cap_element.HT_caps_info;
}

void rtw_rust_ap_sta_info_set_sm_ps(_adapter *padapter, u8 sm_ps)
{
	padapter->mlmeextpriv.mlmext_info.SM_PS = sm_ps;
}

void rtw_rust_ap_sta_info_set_hw_ampdu_min_space(_adapter *padapter, u8 val)
{
	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_MIN_SPACE, &val);
}

void rtw_rust_ap_sta_info_set_hw_ampdu_factor(_adapter *padapter, u8 val)
{
	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_FACTOR, &val);
}

#endif /* CONFIG_RUST_AP_STA_INFO && !HOST_AP_STA_INFO_TEST */
