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
#define _RTW_XMIT_UPDATE_ATTRIB_SEC_REST_C_

#include <drv_types.h>

#if defined(CONFIG_RUST)

struct rtw_rust_xmit_sec_gather {
	u8 ieee8021x_blocked;
	u32 passing_ms;
	int eapol_type;
	u16 ether_type;
	u8 wifi_mp_state;
	u8 bmcast;
	u8 dot11_auth_algrthm;
	u8 dot11_privacy_algrthm;
	u8 dot118021x_grp_privacy;
	u8 sta_dot118021x_privacy;
	u8 dot11_privacy_key_index;
	u8 dot118021x_grp_keyid;
	u8 direct_link;
};

struct rtw_rust_xmit_sec_decision {
	s32 res;
	u8 encrypt;
	u8 key_idx;
};

void update_attrib_sec_info_decide_rust(const struct rtw_rust_xmit_sec_gather *in,
					struct rtw_rust_xmit_sec_decision *out);

static void rtw_rust_xmit_attrib_sec_gather(_adapter *padapter, struct pkt_attrib *pattrib,
					    struct sta_info *psta, enum eap_type eapol_type,
					    struct rtw_rust_xmit_sec_gather *out)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	sint bmcast = IS_MCAST(pattrib->ra);

	out->ieee8021x_blocked = psta->ieee8021x_blocked;
	out->passing_ms = rtw_get_passing_time_ms(psta->resp_nonenc_eapol_key_starttime);
	out->eapol_type = (int)eapol_type;
	out->ether_type = pattrib->ether_type;
	out->wifi_mp_state = check_fwstate(pmlmepriv, WIFI_MP_STATE) ? 1 : 0;
	out->bmcast = bmcast ? 1 : 0;
	out->dot11_auth_algrthm = (u8)psecuritypriv->dot11AuthAlgrthm;
	out->dot11_privacy_algrthm = (u8)psecuritypriv->dot11PrivacyAlgrthm;
	out->dot118021x_grp_privacy = (u8)psecuritypriv->dot118021XGrpPrivacy;
	out->sta_dot118021x_privacy = (u8)psta->dot118021XPrivacy;
	out->dot11_privacy_key_index = (u8)psecuritypriv->dot11PrivacyKeyIndex;
	out->dot118021x_grp_keyid = (u8)psecuritypriv->dot118021XGrpKeyid;
#ifdef CONFIG_TDLS
	out->direct_link = pattrib->direct_link;
#else
	out->direct_link = 0;
#endif
}

#endif /* CONFIG_RUST */

s32 update_attrib_sec_info(_adapter *padapter, struct pkt_attrib *pattrib,
			   struct sta_info *psta, enum eap_type eapol_type)
{
	sint res = _SUCCESS;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	sint bmcast = IS_MCAST(pattrib->ra);

	_rtw_memset(pattrib->dot118021x_UncstKey.skey, 0, 16);
	_rtw_memset(pattrib->dot11tkiptxmickey.skey, 0, 16);
	pattrib->mac_id = psta->cmn.mac_id;

#if defined(CONFIG_RUST)
	{
		struct rtw_rust_xmit_sec_gather gather;
		struct rtw_rust_xmit_sec_decision decision;

		rtw_rust_xmit_attrib_sec_gather(padapter, pattrib, psta, eapol_type, &gather);
		update_attrib_sec_info_decide_rust(&gather, &decision);
		res = decision.res;
		pattrib->encrypt = decision.encrypt;
		pattrib->key_idx = decision.key_idx;
		if (res != _SUCCESS)
			goto exit;
	}
#else
	/* Comment by Owen at 2020/05/19 — see git history for full 4-way EAPOL note. */
	if (psta->ieee8021x_blocked == _TRUE ||
	    ((eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4) &&
	     rtw_get_passing_time_ms(psta->resp_nonenc_eapol_key_starttime) <= 100)) {

		if (eapol_type == EAPOL_2_4 || eapol_type == EAPOL_4_4)
			RTW_INFO("Respond unencrypted eapol key\n");

		pattrib->encrypt = 0;

		if ((pattrib->ether_type != 0x888e) &&
		    (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _FALSE)) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psta->ieee8021x_blocked == _TRUE,  pattrib->ether_type(%04x) != 0x888e\n", __FUNCTION__, pattrib->ether_type);
#endif
			res = _FAIL;
			goto exit;
		}
	} else {
		GET_ENCRY_ALGO(psecuritypriv, psta, pattrib->encrypt, bmcast);

#ifdef CONFIG_WAPI_SUPPORT
		if (pattrib->ether_type == 0x88B4)
			pattrib->encrypt = _NO_PRIVACY_;
#endif

		switch (psecuritypriv->dot11AuthAlgrthm) {
		case dot11AuthAlgrthm_Open:
		case dot11AuthAlgrthm_Shared:
		case dot11AuthAlgrthm_Auto:
			pattrib->key_idx = (u8)psecuritypriv->dot11PrivacyKeyIndex;
			break;
		case dot11AuthAlgrthm_8021X:
			if (bmcast)
				pattrib->key_idx = (u8)psecuritypriv->dot118021XGrpKeyid;
			else
				pattrib->key_idx = 0;
			break;
		default:
			pattrib->key_idx = 0;
			break;
		}

		if (((pattrib->encrypt == _WEP40_) || (pattrib->encrypt == _WEP104_)) &&
		    (pattrib->ether_type == 0x888e))
			pattrib->encrypt = _NO_PRIVACY_;
	}

#ifdef CONFIG_TDLS
	if (pattrib->direct_link == _TRUE) {
		if (pattrib->encrypt > 0)
			pattrib->encrypt = _AES_;
	}
#endif
#endif /* !CONFIG_RUST */

	switch (pattrib->encrypt) {
	case _WEP40_:
	case _WEP104_:
		pattrib->iv_len = 4;
		pattrib->icv_len = 4;
		WEP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		break;

	case _TKIP_:
		pattrib->iv_len = 8;
		pattrib->icv_len = 4;

		if (psecuritypriv->busetkipkey == _FAIL) {
#ifdef DBG_TX_DROP_FRAME
			RTW_INFO("DBG_TX_DROP_FRAME %s psecuritypriv->busetkipkey(%d)==_FAIL drop packet\n", __FUNCTION__, psecuritypriv->busetkipkey);
#endif
			res = _FAIL;
			goto exit;
		}

		if (bmcast)
			TKIP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			TKIP_IV(pattrib->iv, psta->dot11txpn, 0);

		_rtw_memcpy(pattrib->dot11tkiptxmickey.skey, psta->dot11tkiptxmickey.skey, 16);

		break;

	case _AES_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 8;

		if (bmcast)
			AES_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			AES_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

	case _GCMP_:
	case _GCMP_256_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 16;

		if (bmcast)
			GCMP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			GCMP_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

	case _CCMP_256_:

		pattrib->iv_len = 8;
		pattrib->icv_len = 16;

		if (bmcast)
			GCMP_IV(pattrib->iv, psta->dot11txpn, pattrib->key_idx);
		else
			GCMP_IV(pattrib->iv, psta->dot11txpn, 0);

		break;

#ifdef CONFIG_WAPI_SUPPORT
	case _SMS4_:
		pattrib->iv_len = 18;
		pattrib->icv_len = 16;
		rtw_wapi_get_iv(padapter, pattrib->ra, pattrib->iv);
		break;
#endif
	default:
		pattrib->iv_len = 0;
		pattrib->icv_len = 0;
		break;
	}

	if (pattrib->encrypt > 0) {
		_rtw_memcpy(pattrib->dot118021x_UncstKey.skey, psta->dot118021x_UncstKey.skey,
			    (pattrib->encrypt & _SEC_TYPE_256_) ? 32 : 16);
	}

	if (pattrib->encrypt &&
	    ((padapter->securitypriv.sw_encrypt == _TRUE) ||
	     (psecuritypriv->hw_decrypted == _FALSE))) {
		pattrib->bswenc = _TRUE;
	} else {
		pattrib->bswenc = _FALSE;
	}

	pattrib->bmc_camid = padapter->securitypriv.dot118021x_bmc_cam_id;

	if (pattrib->encrypt && bmcast &&
	    _rtw_camctl_chk_flags(padapter, SEC_STATUS_STA_PK_GK_CONFLICT_DIS_BMC_SEARCH))
		pattrib->bswenc = _TRUE;

#ifdef CONFIG_WAPI_SUPPORT
	if (pattrib->encrypt == _SMS4_)
		pattrib->bswenc = _FALSE;
#endif

	if ((pattrib->encrypt) && (eapol_type == EAPOL_4_4))
		pattrib->bswenc = _TRUE;

exit:

	return res;
}
