// SPDX-License-Identifier: GPL-2.0
/* Host C oracle for update_attrib_vcs_info (W3-86 PR1). */
#include "host_xmit_update_attrib_types.h"

static u8 validate_vcs(_adapter *padapter, u8 mode)
{
	switch (padapter->registrypriv.vrtl_carrier_sense) {
	case DISABLE_VCS:
		return NONE_VCS;
	case ENABLE_VCS:
		return padapter->registrypriv.vcs_type;
	case AUTO_VCS:
		return mode;
	default:
		return NONE_VCS;
	}
}

void update_attrib_vcs_info(_adapter *padapter, struct xmit_frame *pxmitframe)
{
	u32 sz;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;

	sz = (pattrib->nr_frags != 1) ? padapter->xmitpriv.frag_len
				      : pattrib->last_txcmdsz;

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
			if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_ATHEROS &&
			    pattrib->ampdu_en == _TRUE &&
			    padapter->securitypriv.dot11PrivacyAlgrthm == _AES_) {
				pattrib->vcs_mode = CTS_TO_SELF;
				break;
			}
			if (pattrib->rtsen || pattrib->cts2self) {
				pattrib->vcs_mode = pattrib->rtsen ? RTS_CTS
								   : CTS_TO_SELF;
				break;
			}
			if (pattrib->ht_en) {
				u8 ht_op = pmlmeinfo->HT_protection;

				if ((pmlmeext->cur_bwmode &&
				     (ht_op == 2 || ht_op == 3)) ||
				    (!pmlmeext->cur_bwmode && ht_op == 3)) {
					pattrib->vcs_mode = RTS_CTS;
					break;
				}
			}
			if (sz > padapter->registrypriv.rts_thresh ||
			    (pattrib->ampdu_en == _TRUE &&
			     !IS_HARDWARE_TYPE_8812(padapter))) {
				pattrib->vcs_mode = RTS_CTS;
				break;
			}
			pattrib->vcs_mode = NONE_VCS;
			break;
		}
	}

	pattrib->vcs_mode = validate_vcs(padapter, pattrib->vcs_mode);
	if (padapter->driver_vcs_en == 1)
		pattrib->vcs_mode = padapter->driver_vcs_type;
}
