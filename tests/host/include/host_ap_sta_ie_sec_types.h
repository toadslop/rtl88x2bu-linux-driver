/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_IE_SEC_TYPES_H
#define HOST_AP_STA_IE_SEC_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define BIT(x) (1U << (x))
#define CHECK_BIT(a, b) (!!((a) & (b)))

#define _STATS_SUCCESSFUL_ 0
#define _STATS_UNABLE_HANDLE_STA_ 17
#define _NO_PRIVACY_ 0

#define WPS_ATTR_SELECTED_REGISTRAR 0x1041

#define HOST_WIFI_AP_STATE 0x00000010
#define CHK_MLME_STATE(adpt, st) \
	((((adpt)->mlmepriv.cur_network.state) & (st)) != 0)
#define MLME_IS_AP(adpt) CHK_MLME_STATE((adpt), HOST_WIFI_AP_STATE)

#define RTW_INFO(...) do { } while (0)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(addr) \
	(addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define MFP_NO 0
#define MFP_OPTIONAL 2
#define MFP_REQUIRED 3

#define WLAN_STA_WPS (1U << 12)
#define WLAN_STA_MAYBE_WPS (1U << 13)
#define WLAN_STA_MFP BIT(10)
#define WLAN_STA_AMSDU_DISABLE BIT(17)

#define WLAN_AKM_TYPE_SAE BIT(7)

#define WPA_CIPHER_TKIP BIT(3)
#define WPA_CIPHER_CCMP BIT(4)

#define WLAN_STATUS_INVALID_IE 40
#define WLAN_STATUS_GROUP_CIPHER_NOT_VALID 41
#define WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID 42
#define WLAN_STATUS_AKMP_NOT_VALID 43
#define WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION 31
#define WLAN_STATUS_CIPHER_REJECTED_PER_POLICY 46

enum security_type {
	SEC_TYPE_NONE = 0,
};

struct cmn_sta_info {
	u8 mac_addr[6];
};

struct sta_info {
	struct cmn_sta_info cmn;
	u8 dot8021xalg;
	u8 wpa_psk;
	int wpa_group_cipher;
	int wpa2_group_cipher;
	int wpa_pairwise_cipher;
	int wpa2_pairwise_cipher;
	u32 akm_suite_type;
	u8 authalg;
	int flags;
	u8 wpa_ie[256];
};

struct security_priv {
	u8 wpa_psk;
	unsigned int wpa_group_cipher;
	unsigned int wpa_pairwise_cipher;
	unsigned int wpa2_group_cipher;
	unsigned int wpa2_pairwise_cipher;
	u8 dot11PrivacyAlgrthm;
	u8 mfp_opt;
	u32 akmp;
	u8 auth_type;
	enum security_type dot11wCipher;
};

struct registry_priv {
	u8 amsdu_mode;
};

struct mlme_ext_priv {
	u8 pad;
};

struct wlan_network {
	u32 state;
};

struct mlme_priv {
	struct wlan_network cur_network;
	u8 *wps_beacon_ie;
	u16 wps_beacon_ie_len;
};

struct _adapter {
	struct security_priv securitypriv;
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
};

typedef struct _adapter _adapter;

struct rtw_ieee802_11_elems {
	u8 *rsn_ie;
	u8 rsn_ie_len;
	u8 *wpa_ie;
	u8 wpa_ie_len;
	u8 *wps_ie;
	u8 wps_ie_len;
};

int rtw_parse_wpa_ie(u8 *wpa_ie, int wpa_ie_len, int *group_cipher,
		     int *pairwise_cipher, u32 *akm);
int rtw_parse_wpa2_ie(u8 *rsn_ie, int rsn_ie_len, int *group_cipher,
		      int *pairwise_cipher, int *gmcs, u32 *akm, u8 *mfp_opt,
		      u8 *spp_opt);
u8 rtw_check_amsdu_disable(u8 mode, u8 spp_opt);
u32 security_type_bip_to_gmcs(enum security_type type);
u8 *rtw_get_wps_attr_content(u8 *wps_ie, unsigned int wps_ielen,
			     u16 target_attr_id, u8 *buf_content,
			     unsigned int *len_content);

u16 rtw_ap_parse_sta_security_ie(_adapter *adapter, struct sta_info *sta,
				 struct rtw_ieee802_11_elems *elems);

#endif /* HOST_AP_STA_IE_SEC_TYPES_H */
