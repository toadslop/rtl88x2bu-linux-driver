/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BCN_UPDATE_TYPES_H
#define HOST_AP_BCN_UPDATE_TYPES_H

#include "host_types.h"

#define _BEACON_IE_OFFSET_ 12
#define _ERPINFO_IE_ 42
#define RTW_ERP_INFO_NON_ERP_PRESENT (1U << 0)
#define RTW_ERP_INFO_USE_PROTECTION (1U << 1)
#define RTW_ERP_INFO_BARKER_PREAMBLE_MODE (1U << 2)
#define RTW_INFO(...) do { } while (0)

typedef int sint;
typedef struct _NDIS_802_11_VARIABLE_IEs {
	u8 ElementID;
	u8 Length;
	u8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

typedef struct _WLAN_BSSID_EX {
	u32 IELength;
	u8 IEs[256];
} WLAN_BSSID_EX;

struct mlme_ext_info {
	u8 ERP_enable;
	WLAN_BSSID_EX network;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
};

struct mlme_priv {
	int num_sta_non_erp;
	int num_sta_no_short_preamble;
};

typedef struct _adapter {
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
} _adapter;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void ERP_IE_handler(_adapter *padapter, PNDIS_802_11_VARIABLE_IEs pIE);
void update_bcn_erpinfo_ie(_adapter *padapter);

#endif /* HOST_AP_BCN_UPDATE_TYPES_H */
