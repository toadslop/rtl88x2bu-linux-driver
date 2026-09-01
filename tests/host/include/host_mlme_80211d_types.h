/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_80211D_TYPES_H
#define HOST_MLME_80211D_TYPES_H

#include "host_types.h"
#include <stdlib.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define MAX_IE_SZ 768
#define MAX_CHANNEL_NUM 59
#define _FIXED_IE_LENGTH_ 12
#define _COUNTRY_IE_ 7
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define RTW_CHF_NO_IR BIT0
#define WIRELESS_11G BIT1
#define WIRELESS_11A BIT2
#define RTW_INFO(...) do { } while (0)

typedef int sint;
typedef unsigned int uint;

typedef struct {
	u8 ChannelNum;
	u8 flags;
} RT_CHANNEL_INFO;

typedef struct {
	u8 Channel[MAX_CHANNEL_NUM];
	u8 Len;
} RT_CHANNEL_PLAN;

typedef struct {
	u32 SsidLength;
	u8 Ssid[32];
} NDIS_802_11_SSID;

typedef struct {
	u32 Length;
	u8 MacAddress[6];
	u8 Reserved[2];
	NDIS_802_11_SSID Ssid;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

struct registry_priv {
	u8 enable80211d;
	u8 wireless_mode;
};

struct mlme_ext_priv {
	u8 update_channel_plan_by_ap_done;
};

struct rf_ctl_t {
	RT_CHANNEL_INFO channel_set[MAX_CHANNEL_NUM];
};

struct _adapter {
	struct registry_priv registrypriv;
	struct mlme_ext_priv mlmeextpriv;
	struct rf_ctl_t rfctl;
};

typedef struct _adapter _adapter;
typedef _adapter *PADAPTER;

#define adapter_to_rfctl(a) (&(a)->rfctl)

static inline void *rtw_malloc(size_t sz) { return malloc(sz); }
static inline void rtw_mfree(void *p, size_t sz) { (void)sz; free(p); }
static inline void rtw_nlrtw_reg_change_event(_adapter *a) { (void)a; }

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void process_80211d(PADAPTER padapter, WLAN_BSSID_EX *bssid);

#endif /* HOST_MLME_80211D_TYPES_H */
