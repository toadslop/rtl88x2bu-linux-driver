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
#define _RTW_XMIT_UPDATE_ATTRIB_REST_C_

#ifdef HOST_XMIT_UPDATE_ATTRIB_TEST
#include "host_xmit_update_attrib_types.h"
#else
#include <drv_types.h>
#endif

static u8 validate_vcs(_adapter *padapter, u8 mode)
{
	u8 vcs_mode = NONE_VCS;

	switch (padapter->registrypriv.vrtl_carrier_sense) {
	case DISABLE_VCS:
		vcs_mode = NONE_VCS;
		break;
	case ENABLE_VCS:
		vcs_mode = padapter->registrypriv.vcs_type;
		break;
	case AUTO_VCS:
		vcs_mode = mode;
		break;
	default:
		vcs_mode = NONE_VCS;
		break;
	}

	return vcs_mode;
}

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	u32 sz;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	#ifdef RTW_FORCE_CTS_TO_SELF_UNDER_LOW_RSSI
	s8 rssi = 0;
	struct sta_info *psta = pattrib->psta;
	#endif

	if (pattrib->nr_frags != 1)
		sz = padapter->xmitpriv.frag_len;
	else
		sz = pattrib->last_txcmdsz;

	if (pmlmeext->cur_wireless_mode < WIRELESS_11_24N ||
	    padapter->registrypriv.wifi_spec) {
		if (sz > padapter->registrypriv.rts_thresh)
			pattrib->vcs_mode = RTS_CTS;
		else if (pattrib->rtsen)
			pattrib->vcs_mode = RTS_CTS;
		else if (pattrib->cts2self)
			pattrib->vcs_mode = CTS_TO_SELF;
		else
			pattrib->vcs_mode = NONE_VCS;
	} else {
		while (_TRUE) {
			if ((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_ATHEROS) &&
			    (pattrib->ampdu_en == _TRUE) &&
			    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
				pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}

			if (pattrib->rtsen || pattrib->cts2self) {
				if (pattrib->rtsen)
					pattrib->vcs_mode = RTS_CTS;
				else if (pattrib->cts2self)
					pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}

			if (pattrib->ht_en) {
				u8 HTOpMode = pmlmeinfo->HT_protection;

				if ((pmlmeext->cur_bwmode &&
				     (HTOpMode == 2 || HTOpMode == 3)) ||
				    (!pmlmeext->cur_bwmode && HTOpMode == 3)) {
					pattrib->vcs_mode = RTS_CTS;
					break;
				}
			}

			if (sz > padapter->registrypriv.rts_thresh) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			if ((pattrib->ampdu_en == _TRUE) &&
			    (!IS_HARDWARE_TYPE_8812(padapter))) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}

			pattrib->vcs_mode = NONE_VCS;
			break;
		}
		#ifdef RTW_FORCE_CTS_TO_SELF_UNDER_LOW_RSSI
		if (psta != NULL) {
			rssi = psta->cmn.rssi_stat.rssi;
			if ((rssi < 18) && (pattrib->vcs_mode == RTS_CTS))
				pattrib->vcs_mode = CTS_TO_SELF;
		}
		#endif
	}

	pattrib->vcs_mode = validate_vcs(padapter, pattrib->vcs_mode);

	if (padapter->driver_vcs_en == 1)
		pattrib->vcs_mode = padapter->driver_vcs_type;
}
