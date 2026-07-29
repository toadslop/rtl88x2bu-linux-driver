/* SPDX-License-Identifier: GPL-2.0 */
/*
 * IEEE 802.11 types/constants for host ieee80211_rest L2 tests (W3-26).
 */
#ifndef HOST_IEEE80211_TYPES_H
#define HOST_IEEE80211_TYPES_H

#include "host_types.h"
#include "host_wlan_util_types.h"

#include <stdbool.h>

#define _TRUE 1
#define _FALSE 0
#define _FAIL 0
#define _SUCCESS 1

#define BIT(n) (1U << (n))
#define BIT0 BIT(0)
#define BIT1 BIT(1)
#define BIT2 BIT(2)
#define BIT3 BIT(3)
#define BIT4 BIT(4)
#define BIT5 BIT(5)
#define BIT6 BIT(6)

#define WIRELESS_INVALID 0
#define WIRELESS_11B BIT0
#define WIRELESS_11G BIT1
#define WIRELESS_11A BIT2
#define WIRELESS_11_24N BIT3
#define WIRELESS_11_5N BIT4
#define WIRELESS_11AC BIT6
#define WIRELESS_11BG (WIRELESS_11B | WIRELESS_11G)
#define WIRELESS_11G_24N (WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_11BG_24N (WIRELESS_11B | WIRELESS_11G | WIRELESS_11_24N)
#define WIRELESS_11_5AC BIT6
#define WIRELESS_11A_5N (WIRELESS_11A | WIRELESS_11_5N)

#define NDIS_802_11_LENGTH_RATES_EX 16
#define IEEE80211_CCK_RATE_LEN 4
#define IEEE80211_NUM_OFDM_RATESLEN 8

#define _BEACON_IE_OFFSET_ 12
#define _SUPPORTEDRATES_IE_ 1
#define _EXT_SUPPORTEDRATES_IE_ 50

typedef enum _RATE_SECTION {
	CCK = 0,
	OFDM = 1,
} RATE_SECTION;

typedef unsigned char NDIS_802_11_MAC_ADDRESS[6];
typedef long NDIS_802_11_RSSI;
typedef unsigned char NDIS_802_11_RATES_EX[NDIS_802_11_LENGTH_RATES_EX];

typedef struct _NDIS_802_11_SSID {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct _NDIS_802_11_CONFIGURATION {
	u32 Length;
	u32 BeaconPeriod;
	u32 ATIMWindow;
	u32 DSConfig;
} NDIS_802_11_CONFIGURATION;

typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE {
	Ndis802_11IBSS,
	Ndis802_11Infrastructure,
} NDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef struct _WLAN_PHY_INFO {
	u8 SignalStrength;
	u8 SignalQuality;
	u8 Optimum_antenna;
} WLAN_PHY_INFO;

#define HOST_IEEE80211_MAX_IE_SZ 256

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	u8 Reserved[2];
	NDIS_802_11_SSID Ssid;
	NDIS_802_11_SSID mesh_id;
	u32 Privacy;
	NDIS_802_11_RSSI Rssi;
	NDIS_802_11_CONFIGURATION Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	NDIS_802_11_RATES_EX SupportedRates;
	WLAN_PHY_INFO PhyInfo;
	u32 IELength;
	u8 IEs[HOST_IEEE80211_MAX_IE_SZ];
} WLAN_BSSID_EX;

typedef int sint;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
int rtw_ies_remove_ie(u8 *ies, unsigned int *ies_len, unsigned int offset,
		      u8 eid, u8 *oui, u8 oui_len);
bool rtw_is_cck_rate(u8 rate);
bool rtw_is_ofdm_rate(u8 rate);
bool rtw_is_basic_rate_ofdm(u8 rate);

#endif /* HOST_IEEE80211_TYPES_H */
