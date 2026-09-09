/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_IE_WMM_HT_TYPES_H
#define HOST_AP_STA_IE_WMM_HT_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define BIT(x) (1U << (x))

#define WLAN_EID_VENDOR_SPECIFIC 221
#define WLAN_STA_WME (1U << 9)
#define WLAN_STA_HT (1U << 11)

#define HT_OP_IE_LEN 22

struct rtw_ieee80211_ht_cap {
	u16 cap_info;
	u8 ampdu_params_info;
	u8 supp_mcs_set[16];
	u16 extended_ht_cap_info;
	u32 tx_BF_cap_info;
	u8 antenna_selection_info;
} __attribute__((packed));

struct ht_priv {
	u8 ht_option;
	u8 op_present;
	struct rtw_ieee80211_ht_cap ht_cap;
	u8 ht_op[HT_OP_IE_LEN];
};

struct qos_priv {
	unsigned int qos_option;
};

struct mlme_priv {
	struct qos_priv qospriv;
	struct ht_priv htpriv;
};

struct sta_info {
	int flags;
	u32 qos_option;
	u8 qos_info;
	u8 has_legacy_ac;
	u8 uapsd_vo;
	u8 uapsd_vi;
	u8 uapsd_be;
	u8 uapsd_bk;
	u8 max_sp_len;
	struct ht_priv htpriv;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

struct rtw_ieee802_11_elems {
	u8 *ht_capabilities;
	u8 ht_capabilities_len;
	u8 *ht_operation;
	u8 ht_operation_len;
};

u8 *rtw_get_ie_ex(const u8 *in_ie, unsigned int in_len, u8 eid, const u8 *oui,
		  u8 oui_len, u8 *ie, unsigned int *ielen);

void rtw_ap_parse_sta_wmm_ie(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies,
			     u16 tlv_ies_len);
void rtw_ap_parse_sta_ht_ie(_adapter *adapter, struct sta_info *sta,
			    struct rtw_ieee802_11_elems *elems);

#endif /* HOST_AP_STA_IE_WMM_HT_TYPES_H */
