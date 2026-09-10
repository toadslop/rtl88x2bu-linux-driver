/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BCN_IE_TYPES_H
#define HOST_AP_BCN_IE_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"

#include <stddef.h>
#include <stdint.h>

#define _TRUE 1
#define _FALSE 0
#define _BEACON_IE_OFFSET_ 12
#define _FIXED_IE_LENGTH_ _BEACON_IE_OFFSET_
#define _SSID_IE_ 0
#define _SUPPORTEDRATES_IE_ 1
#define _TIM_IE_ 5
#define _ERPINFO_IE_ 42

#define BIT(n) (1U << (n))
#define BIT0 BIT(0)

typedef unsigned int uint;
typedef int sint;

#define HOST_AP_BCN_IE_MAX_IE_SZ 256

typedef struct _NDIS_802_11_VARIABLE_IEs {
	u8 ElementID;
	u8 Length;
	u8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

typedef struct _NDIS_802_11_FIXED_IEs {
	u8 Timestamp[8];
	u16 BeaconInterval;
	u16 Capabilities;
} NDIS_802_11_FIXED_IEs;

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	u8 MacAddress[6];
	u8 Reserved[2];
	u8 Ssid[32];
	u8 mesh_id[32];
	u32 Privacy;
	s32 Rssi;
	u8 Configuration[16];
	u32 InfrastructureMode;
	u8 SupportedRates[16];
	u8 PhyInfo[4];
	u32 IELength;
	u8 IEs[HOST_AP_BCN_IE_MAX_IE_SZ];
} WLAN_BSSID_EX;

struct sta_priv {
	u8 aid_bmp_len;
	u8 tim_bitmap[8];
};

struct mlme_ext_info {
	WLAN_BSSID_EX network;
};

struct mlme_ext_priv {
	struct mlme_ext_info mlmext_info;
};

typedef struct _adapter {
	struct sta_priv stapriv;
	struct mlme_ext_priv mlmeextpriv;
} _adapter;

u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period, const u8 *tim_bmp, u8 tim_bmp_len,
		  u8 *tim_ie);
void update_BCNTIM(_adapter *padapter);
void rtw_add_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index,
		    u8 *data, u8 len);
void rtw_remove_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index);

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void *rtw_malloc(size_t sz);
void rtw_mfree(void *p, size_t sz);

#endif /* HOST_AP_BCN_IE_TYPES_H */
