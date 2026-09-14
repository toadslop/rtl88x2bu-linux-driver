/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_RA_TYPES_H
#define HOST_AP_STA_RA_TYPES_H

#include "host_types.h"

#define WIRELESS_11B (1U << 0)
#define WIRELESS_11G (1U << 1)
#define WIRELESS_11A (1U << 2)
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define WIRELESS_11AC (1U << 6)
#define WIRELESS_11_5AC (WIRELESS_11A | WIRELESS_11AC)
#define WIFI_ASOC_STATE 0x00000001U

struct rtw_ra_info {
	u64 ramask;
};

struct cmn_sta_info {
	struct rtw_ra_info ra_info;
};

struct vht_priv {
	u8 vht_option;
};

struct sta_info {
	struct cmn_sta_info cmn;
	u32 state;
	u8 wireless_mode;
	struct vht_priv vhtpriv;
};

struct wlan_config {
	u32 DSConfig;
};

typedef struct {
	struct wlan_config Configuration;
} WLAN_BSSID_EX;

struct wlan_network {
	WLAN_BSSID_EX network;
};

struct mlme_priv {
	struct wlan_network cur_network;
};

typedef struct {
	struct mlme_priv mlmepriv;
} _adapter;

void rtw_hal_update_sta_ra_info(_adapter *padapter, struct sta_info *psta);
void rtw_hal_update_sta_wset(_adapter *padapter, struct sta_info *psta);

void rtw_ap_update_sta_ra_info(_adapter *padapter, struct sta_info *psta);

void host_sta_ra_reset(void);
void host_sta_ra_set_hal_ramask(u64 ramask);
u8 host_sta_ra_hal_ra_called(void);
u8 host_sta_ra_hal_wset_called(void);
#endif /* HOST_AP_STA_RA_TYPES_H */
