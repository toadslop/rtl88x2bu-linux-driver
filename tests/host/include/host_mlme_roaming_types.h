/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_ROAMING_TYPES_H
#define HOST_MLME_ROAMING_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include "host_types.h"

#define _SUCCESS 1
#define _FAIL 0
#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define MAX_IE_SZ 768
#define NDIS_802_11_LENGTH_RATES_EX 16

#define RTW_INFO(...) do { } while (0)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]
#define is_zero_mac_addr(Addr) \
	((Addr)[0] == 0x00 && (Addr)[1] == 0x00 && (Addr)[2] == 0x00 && \
	 (Addr)[3] == 0x00 && (Addr)[4] == 0x00 && (Addr)[5] == 0x00)

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
} WLAN_BSSID_EX;

typedef unsigned long systime, _irqL;
typedef int _lock;

struct _list {
	struct _list *next, *prev;
};
typedef struct _list _list;

struct __queue {
	_list queue;
	_lock lock;
};
typedef struct __queue _queue;

typedef struct {
	u8 ChannelNum;
} RT_CHANNEL_INFO;

struct rf_ctl_t {
	RT_CHANNEL_INFO channel_set[14];
	u8 ch_num;
};

struct wlan_network {
	_list list;
	WLAN_BSSID_EX network;
	systime last_scanned;
};

struct mlme_priv {
	_queue scanned_queue;
	struct wlan_network *cur_network_scanned;
	struct wlan_network cur_network;
	u8 need_to_roam;
	u8 roam_tgt_addr[ETH_ALEN];
	u32 roam_scanr_exp_ms;
	s32 roam_rssi_diff_th;
	void *nic_hdl;
	struct wlan_network *roam_network;
	_list *pscanned;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct rf_ctl_t rfctl;
};

typedef struct _adapter _adapter;

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#define adapter_to_rfctl(adapter) (&(adapter)->rfctl)
#define LIST_CONTAINOR(ptr, type, member) \
	container_of(ptr, type, member)
#define IS_DFS_SLAVE_WITH_RD(rfctl) 0
#define rtw_rfctl_dfs_domain_unknown(rfctl) 1
#define rtw_chset_is_ch_non_ocp(chset, ch) 0
#define rtw_warn_on(x) (void)(x)

static inline void _enter_critical_bh(_lock *l, _irqL *i)
{
	(void)l;
	(void)i;
}

static inline void _exit_critical_bh(_lock *l, _irqL *i)
{
	(void)l;
	(void)i;
}

static inline _list *get_list_head(_queue *q)
{
	return &q->queue;
}

static inline _list *get_next(_list *l)
{
	return l->next;
}

static inline int rtw_end_of_queue_search(_list *h, _list *l)
{
	return h == l;
}

static inline void rtw_list_insert_tail(_list *n, _list *h)
{
	n->next = h;
	n->prev = h->prev;
	h->prev->next = n;
	h->prev = n;
}

int _rtw_memcmp(const void *a, const void *b, size_t n);
systime rtw_get_current_time(void);
u32 rtw_get_passing_time_ms(systime start);
int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, u32 ch);
int rtw_is_desired_network(_adapter *adapter, struct wlan_network *pnetwork);
int is_same_ess(WLAN_BSSID_EX *a, WLAN_BSSID_EX *b);
int rtw_check_roaming_candidate(struct mlme_priv *mlme,
				struct wlan_network **candidate,
				struct wlan_network *competitor);
int rtw_select_roaming_candidate(struct mlme_priv *mlme);

#endif /* HOST_MLME_ROAMING_TYPES_H */
