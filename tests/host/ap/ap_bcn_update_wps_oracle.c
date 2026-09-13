// SPDX-License-Identifier: GPL-2.0
/* Host oracle for update_bcn_wps_ie until W3-81 PR5 moves it to core/rtw_ap_bcn_update.c. */
#include "host_ap_bcn_update_types.h"
#include <stdlib.h>
#include <string.h>

u32 host_bcn_update_last_ielen;

void update_bcn_wps_ie(_adapter *padapter)
{
	u8 *pwps_ie = NULL, *pwps_ie_src, *premainder_ie, *pbackup_remainder_ie = NULL;
	u32 wps_ielen = 0, wps_offset, remainder_ielen;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_info *pmlmeinfo = &(padapter->mlmeextpriv.mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	u8 *ie = pnetwork->IEs;
	u32 ielen = pnetwork->IELength;

	pwps_ie = rtw_get_wps_ie(ie + _FIXED_IE_LENGTH_, ielen - _FIXED_IE_LENGTH_,
				 NULL, &wps_ielen);
	if (pwps_ie == NULL || wps_ielen == 0)
		return;

	pwps_ie_src = pmlmepriv->wps_beacon_ie;
	if (pwps_ie_src == NULL)
		return;

	wps_offset = (u32)(pwps_ie - ie);
	premainder_ie = pwps_ie + wps_ielen;
	remainder_ielen = ielen - wps_offset - wps_ielen;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	wps_ielen = (u32)pwps_ie_src[1];
	if ((wps_offset + wps_ielen + 2 + remainder_ielen) <= MAX_IE_SZ) {
		_rtw_memcpy(pwps_ie, pwps_ie_src, wps_ielen + 2);
		pwps_ie += (wps_ielen + 2);
		if (pbackup_remainder_ie)
			_rtw_memcpy(pwps_ie, pbackup_remainder_ie, remainder_ielen);
		pnetwork->IELength = wps_offset + (wps_ielen + 2) + remainder_ielen;
	}

	if (pbackup_remainder_ie)
		rtw_mfree(pbackup_remainder_ie, remainder_ielen);

	host_bcn_update_last_ielen = pnetwork->IELength;
}
