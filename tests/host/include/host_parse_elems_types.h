/* SPDX-License-Identifier: GPL-2.0 */
/* W3-76: types for element parse L2 harness. */
#ifndef HOST_PARSE_ELEMS_TYPES_H
#define HOST_PARSE_ELEMS_TYPES_H

#include "host_types.h"

#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_FH_PARAMS 2
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_CHALLENGE 16
#define WLAN_EID_ERP_INFO 42
#define WLAN_EID_HT_CAP 45
#define WLAN_EID_RSN 48
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_PWR_CAPABILITY 33
#define WLAN_EID_SUPPORTED_CHANNELS 36
#define WLAN_EID_MOBILITY_DOMAIN 54
#define WLAN_EID_FAST_BSS_TRANSITION 55
#define WLAN_EID_TIMEOUT_INTERVAL 56
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_VHT_CAPABILITY 191
#define WLAN_EID_VHT_OPERATION 192
#define WLAN_EID_VHT_OP_MODE_NOTIFY 199
#define WLAN_EID_VENDOR_SPECIFIC 221
#define _EID_RRM_EN_CAP_IE_ 70

#define OUI_MICROSOFT 0x0050f2
#define WME_OUI_TYPE 2
#define WME_OUI_SUBTYPE_INFORMATION_ELEMENT 0
#define WME_OUI_SUBTYPE_PARAMETER_ELEMENT 1
#define WME_OUI_SUBTYPE_TSPEC_ELEMENT 2
#define OUI_BROADCOM 0x00904c
#define VENDOR_HT_CAPAB_OUI_TYPE 0x33

#define RTW_GET_BE24(a) \
	((((u32)(a)[0]) << 16) | (((u32)(a)[1]) << 8) | ((u32)(a)[2]))

struct rtw_ieee802_11_elems {
	u8 *ssid;
	u8 ssid_len;
	u8 *supp_rates;
	u8 supp_rates_len;
	u8 *fh_params;
	u8 fh_params_len;
	u8 *ds_params;
	u8 ds_params_len;
	u8 *cf_params;
	u8 cf_params_len;
	u8 *tim;
	u8 tim_len;
	u8 *ibss_params;
	u8 ibss_params_len;
	u8 *challenge;
	u8 challenge_len;
	u8 *erp_info;
	u8 erp_info_len;
	u8 *ext_supp_rates;
	u8 ext_supp_rates_len;
	u8 *wpa_ie;
	u8 wpa_ie_len;
	u8 *rsn_ie;
	u8 rsn_ie_len;
	u8 *wme;
	u8 wme_len;
	u8 *wme_tspec;
	u8 wme_tspec_len;
	u8 *wps_ie;
	u8 wps_ie_len;
	u8 *power_cap;
	u8 power_cap_len;
	u8 *supp_channels;
	u8 supp_channels_len;
	u8 *mdie;
	u8 mdie_len;
	u8 *ftie;
	u8 ftie_len;
	u8 *timeout_int;
	u8 timeout_int_len;
	u8 *ht_capabilities;
	u8 ht_capabilities_len;
	u8 *ht_operation;
	u8 ht_operation_len;
	u8 *vendor_ht_cap;
	u8 vendor_ht_cap_len;
	u8 *vht_capabilities;
	u8 vht_capabilities_len;
	u8 *vht_operation;
	u8 vht_operation_len;
	u8 *vht_op_mode_notify;
	u8 vht_op_mode_notify_len;
	u8 *rm_en_cap;
	u8 rm_en_cap_len;
};

typedef enum { ParseOK = 0, ParseUnknown = 1, ParseFailed = -1 } ParseRes;

ParseRes rtw_ieee802_11_parse_elems(u8 *start, unsigned int len,
				    struct rtw_ieee802_11_elems *elems,
				    int show_errors);

#endif /* HOST_PARSE_ELEMS_TYPES_H */
