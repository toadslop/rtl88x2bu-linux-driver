/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_IE_VHT_MULTIAP_TYPES_H
#define HOST_AP_STA_IE_VHT_MULTIAP_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define WLAN_EID_VENDOR_SPECIFIC 221
#define MULTI_AP_SUB_ELEM_TYPE 0x06
#define MULTI_AP_BACKHAUL_STA (1U << 7)
#define MULTI_AP_FRONTHAUL_BSS (1U << 5)
#define MULTI_AP_BACKHAUL_BSS (1U << 6)

#define WLAN_STA_VHT (1U << 14)
#define WLAN_STA_WDS (1U << 15)
#define WLAN_STA_MULTI_AP (1U << 16)

#define VHT_CAP_IE_LEN 12
#define VHT_OP_IE_LEN 5

struct vht_priv {
	u8 vht_option;
	u8 op_present;
	u8 notify_present;
	u8 vht_cap[32];
	u8 vht_op[VHT_OP_IE_LEN];
	u8 vht_op_mode_notify;
};

struct mlme_priv {
	struct vht_priv vhtpriv;
};

struct sta_info {
	int flags;
	struct vht_priv vhtpriv;
};

struct _adapter {
	u8 multi_ap;
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

struct rtw_ieee802_11_elems {
	u8 *vht_capabilities;
	u8 vht_capabilities_len;
	u8 *vht_operation;
	u8 vht_operation_len;
	u8 *vht_op_mode_notify;
	u8 vht_op_mode_notify_len;
};

u8 *rtw_get_ie_ex(const u8 *in_ie, unsigned int in_len, u8 eid, const u8 *oui,
		  u8 oui_len, u8 *ie, unsigned int *ielen);
u8 rtw_get_multi_ap_ie_ext(const u8 *ies, int ies_len);

void rtw_ap_parse_sta_vht_ie(_adapter *adapter, struct sta_info *sta,
			     struct rtw_ieee802_11_elems *elems);
void rtw_ap_parse_sta_multi_ap_ie(_adapter *adapter, struct sta_info *sta, u8 *ies,
				  int ies_len);

#endif /* HOST_AP_STA_IE_VHT_MULTIAP_TYPES_H */
