/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 rtw_mlme_rest tests (W3-53).
 */
#ifndef HOST_MLME_TYPES_H
#define HOST_MLME_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0

#define ETH_ALEN 6
#define MAX_IE_SZ 768
#define NDIS_802_11_LENGTH_RATES_EX 16

#define WLAN_CAPABILITY_BSS (1 << 0)
#define WLAN_CAPABILITY_IBSS (1 << 1)

#define _NO_PRIVACY_ 0x00

#define le16_to_cpu(x) (x)

typedef unsigned char NDIS_802_11_MAC_ADDRESS[ETH_ALEN];
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
	u8 is_cck_rate;
	s8 rx_snr[4];
} WLAN_PHY_INFO;

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
	u8 IEs[MAX_IE_SZ];
} __attribute__((packed)) WLAN_BSSID_EX;

struct wlan_network {
	WLAN_BSSID_EX network;
};

struct security_priv {
	u32 dot11PrivacyAlgrthm;
};

struct _adapter {
	struct security_priv securitypriv;
};

typedef struct _adapter _adapter;

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
int rtw_bug_check(void *parg1, void *parg2, void *parg3, void *parg4);
int is_all_null(char *c, int len);
u32 rtw_random32(void);

void rtw_generate_random_ibss(u8 *pibss);
u8 *rtw_get_capability_from_ie(u8 *ie);
u16 rtw_get_capability(WLAN_BSSID_EX *bss);
u8 *rtw_get_timestampe_from_ie(u8 *ie);
u8 *rtw_get_beacon_interval_from_ie(u8 *ie);
int rtw_is_same_ibss(_adapter *adapter, struct wlan_network *pnetwork);
int is_same_ess(WLAN_BSSID_EX *a, WLAN_BSSID_EX *b);
int is_same_network(WLAN_BSSID_EX *src, WLAN_BSSID_EX *dst, u8 feature);

#endif /* HOST_MLME_TYPES_H */
