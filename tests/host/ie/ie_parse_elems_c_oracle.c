// SPDX-License-Identifier: GPL-2.0
/* C oracle: element parse from core/rtw_ieee80211.c (W3-76).
 *
 * Default host build only: omits kernel #ifdef CONFIG_RTW_MESH (PREQ/PREP/PERR/RANN
 * mesh IEs) and CONFIG_RTW_TOKEN_BASED_XMIT (Realtek TBTX vendor IE) branches.
 */
#include "host_parse_elems_types.h"

#define RTW_INFO(...) do { } while (0)
#define RTW_DBG(...) do { } while (0)

typedef unsigned int uint;

#define IE_FIELD(field) \
	do { \
		elems->field = pos; \
		elems->field##_len = (u8)elen; \
	} while (0)

static int parse_vendor(u8 *pos, uint elen, struct rtw_ieee802_11_elems *elems,
			int show_errors)
{
	unsigned int oui;

	if (elen < 4) {
		if (show_errors)
			RTW_INFO("short vendor IE (len=%lu)\n", (unsigned long)elen);
		return -1;
	}
	oui = RTW_GET_BE24(pos);
	if (oui == OUI_MICROSOFT) {
		if (pos[3] == 1) {
			IE_FIELD(wpa_ie);
			return 0;
		}
		if (pos[3] == WME_OUI_TYPE) {
			if (elen < 5)
				return -1;
			if (pos[4] == WME_OUI_SUBTYPE_INFORMATION_ELEMENT ||
			    pos[4] == WME_OUI_SUBTYPE_PARAMETER_ELEMENT)
				IE_FIELD(wme);
			else if (pos[4] == WME_OUI_SUBTYPE_TSPEC_ELEMENT)
				IE_FIELD(wme_tspec);
			else
				return -1;
			return 0;
		}
		if (pos[3] == 4) {
			IE_FIELD(wps_ie);
			return 0;
		}
		return -1;
	}
	if (oui == OUI_BROADCOM && pos[3] == VENDOR_HT_CAPAB_OUI_TYPE) {
		IE_FIELD(vendor_ht_cap);
	 return 0;
	}
	return -1;
}

ParseRes rtw_ieee802_11_parse_elems(u8 *start, uint len,
				    struct rtw_ieee802_11_elems *elems,
				    int show_errors)
{
	uint left = len;
	u8 *pos = start;
	int unknown = 0;

	_rtw_memset(elems, 0, sizeof(*elems));
	while (left >= 2) {
		u8 id = *pos++;
		u8 elen = *pos++;

		left -= 2;
		if (elen > left) {
			if (show_errors)
				RTW_INFO("parse failed id=%d elen=%d left=%lu\n", id,
					 elen, (unsigned long)left);
			return ParseFailed;
		}
		switch (id) {
		case WLAN_EID_SSID: IE_FIELD(ssid); break;
		case WLAN_EID_SUPP_RATES: IE_FIELD(supp_rates); break;
		case WLAN_EID_FH_PARAMS: IE_FIELD(fh_params); break;
		case WLAN_EID_DS_PARAMS: IE_FIELD(ds_params); break;
		case WLAN_EID_CF_PARAMS: IE_FIELD(cf_params); break;
		case WLAN_EID_TIM: IE_FIELD(tim); break;
		case WLAN_EID_IBSS_PARAMS: IE_FIELD(ibss_params); break;
		case WLAN_EID_CHALLENGE: IE_FIELD(challenge); break;
		case WLAN_EID_ERP_INFO: IE_FIELD(erp_info); break;
		case WLAN_EID_EXT_SUPP_RATES: IE_FIELD(ext_supp_rates); break;
		case WLAN_EID_VENDOR_SPECIFIC:
			if (parse_vendor(pos, elen, elems, show_errors))
				unknown++;
			break;
		case WLAN_EID_RSN: IE_FIELD(rsn_ie); break;
		case WLAN_EID_PWR_CAPABILITY: IE_FIELD(power_cap); break;
		case WLAN_EID_SUPPORTED_CHANNELS: IE_FIELD(supp_channels); break;
		case WLAN_EID_MOBILITY_DOMAIN: IE_FIELD(mdie); break;
		case WLAN_EID_FAST_BSS_TRANSITION: IE_FIELD(ftie); break;
		case WLAN_EID_TIMEOUT_INTERVAL: IE_FIELD(timeout_int); break;
		case WLAN_EID_HT_CAP: IE_FIELD(ht_capabilities); break;
		case WLAN_EID_HT_OPERATION: IE_FIELD(ht_operation); break;
		case WLAN_EID_VHT_CAPABILITY: IE_FIELD(vht_capabilities); break;
		case WLAN_EID_VHT_OPERATION: IE_FIELD(vht_operation); break;
		case WLAN_EID_VHT_OP_MODE_NOTIFY: IE_FIELD(vht_op_mode_notify); break;
		case _EID_RRM_EN_CAP_IE_: IE_FIELD(rm_en_cap); break;
		default:
			unknown++;
			break;
		}
		left -= elen;
		pos += elen;
	}
	return left ? ParseFailed : (unknown ? ParseUnknown : ParseOK);
}
