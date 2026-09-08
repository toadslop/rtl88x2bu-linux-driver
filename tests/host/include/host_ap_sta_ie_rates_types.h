/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_IE_RATES_TYPES_H
#define HOST_AP_STA_IE_RATES_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0

#define WLAN_STA_NONERP (1U << 31)

#define HOST_WIFI_AP_STATE 0x00000010
#define CHK_MLME_STATE(adpt, st) \
	((((adpt)->mlmepriv.cur_network.state) & (st)) != 0)
#define MLME_IS_AP(adpt) CHK_MLME_STATE((adpt), HOST_WIFI_AP_STATE)

#define RTW_INFO(...) do { } while (0)
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(adapter) (adapter)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(addr) \
	(addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define _SUPPORTEDRATES_IE_ 1
#define _EXT_SUPPORTEDRATES_IE_ 50
#define _STATS_SUCCESSFUL_ 0
#define _STATS_FAILURE_ 1

#define IEEE80211_CCK_RATE_1MB 0x02
#define IEEE80211_CCK_RATE_2MB 0x04
#define IEEE80211_CCK_RATE_5MB 0x0b
#define IEEE80211_CCK_RATE_11MB 0x16
#define IEEE80211_OFDM_RATE_6MB 0x0c
#define IEEE80211_OFDM_RATE_9MB 0x12
#define IEEE80211_OFDM_RATE_12MB 0x18
#define IEEE80211_OFDM_RATE_18MB 0x24
#define IEEE80211_OFDM_RATE_24MB 0x30
#define IEEE80211_OFDM_RATE_36MB 0x48
#define IEEE80211_OFDM_RATE_48MB 0x60
#define IEEE80211_OFDM_RATE_54MB 0x6c
#define IEEE80211_BASIC_RATE_MASK 0x80

struct cmn_sta_info {
	u8 mac_addr[6];
};

struct sta_info {
	struct cmn_sta_info cmn;
	u16 capability;
	int flags;
	u8 bssrateset[16];
	u32 bssratelen;
};

struct mlme_priv {
	struct {
		u32 state;
	} cur_network;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

void UpdateBrateTblForSoftAP(u8 *bssrateset, u32 bssratelen);
int rtw_ies_get_supported_rate(u8 *ies, unsigned int ies_len, u8 *rate_set,
			       u8 *rate_num);

u16 rtw_ap_parse_sta_supported_rates(_adapter *adapter, struct sta_info *sta,
				     u8 *tlv_ies, u16 tlv_ies_len);

#endif /* HOST_AP_STA_IE_RATES_TYPES_H */
