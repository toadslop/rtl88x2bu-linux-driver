// SPDX-License-Identifier: GPL-2.0
#define _RTW_AP_BCN_UPDATE_C_

#ifdef HOST_AP_BCN_UPDATE_TEST
#include "host_ap_bcn_update_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST_AP_BCN_UPDATE) || defined(HOST_AP_BCN_UPDATE_TEST)

#ifdef HOST_AP_BCN_UPDATE_TEST
extern u8 host_bcn_update_last_erp_byte;

void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE)
{
	(void)padapter;
	if (pIE && pIE->Length > 0)
		host_bcn_update_last_erp_byte = pIE->data[0];
}
#endif

void update_bcn_erpinfo_ie(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
#ifdef HOST_AP_BCN_UPDATE_TEST
	sint len = 0;
#else
	u32 len = 0;
#endif

#ifndef HOST_AP_BCN_UPDATE_TEST
	RTW_INFO("%s, ERP_enable=%d\n", __FUNCTION__, pmlmeinfo->ERP_enable);
#endif

	if (!pmlmeinfo->ERP_enable)
		return;

	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _ERPINFO_IE_, &len,
		       (pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		PNDIS_802_11_VARIABLE_IEs pIE = (PNDIS_802_11_VARIABLE_IEs)p;

		if (pmlmepriv->num_sta_non_erp == 1)
			pIE->data[0] |= RTW_ERP_INFO_NON_ERP_PRESENT |
					 RTW_ERP_INFO_USE_PROTECTION;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_NON_ERP_PRESENT |
					  RTW_ERP_INFO_USE_PROTECTION);

		if (pmlmepriv->num_sta_no_short_preamble > 0)
			pIE->data[0] |= RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_BARKER_PREAMBLE_MODE);

		ERP_IE_handler(padapter, pIE);
	}
}

#endif /* !CONFIG_RUST_AP_BCN_UPDATE || HOST_AP_BCN_UPDATE_TEST */
