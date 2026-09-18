/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_MLME_JOIN_SELECT_TYPES_H
#define HOST_MLME_JOIN_SELECT_TYPES_H

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

#define WIFI_AP_STATE 0x00000010
#define WIFI_UNDER_LINKING 0x00000080
#define WIFI_UNDER_WPS 0x00000100
#define WIFI_UNDER_SURVEY 0x00000800
#define WIFI_ASOC_STATE 0x00000001

#define SS_DENY_BLOCK_SCAN 2
#define SS_DENY_BY_DRV 3
#define SS_DENY_SELF_AP_UNDER_WPS 4
#define SS_DENY_SELF_AP_UNDER_LINKING 5
#define SS_DENY_SELF_AP_UNDER_SURVEY 6
#define SS_DENY_SELF_STA_UNDER_LINKING 8
#define SS_DENY_SELF_STA_UNDER_SURVEY 9
#define SS_ALLOW 12

#define RTW_INFO(...) do { } while (0)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]
#define rtw_to_roam(adapter) 0

typedef unsigned char NDIS_802_11_MAC_ADDRESS[ETH_ALEN];
typedef long NDIS_802_11_RSSI;

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

typedef struct _WLAN_PHY_INFO {
	u8 SignalStrength;
	u8 SignalQuality;
	u8 Optimum_antenna;
} WLAN_PHY_INFO;

typedef struct _WLAN_BSSID_EX {
	u32 Length;
	NDIS_802_11_MAC_ADDRESS MacAddress;
	u8 Reserved[2];
	NDIS_802_11_SSID Ssid;
	u32 Privacy;
	NDIS_802_11_RSSI Rssi;
	NDIS_802_11_CONFIGURATION Configuration;
	WLAN_PHY_INFO PhyInfo;
	u32 IELength;
	u8 IEs[MAX_IE_SZ];
} WLAN_BSSID_EX;

typedef unsigned long systime, _irqL;
typedef int _lock, sint;

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
};

struct wlan_network {
	_list list;
	WLAN_BSSID_EX network;
	systime last_scanned;
};

struct registry_priv {
	u32 scan_interval_thr;
};

struct dvobj_priv {
	u8 scan_deny;
};

struct mlme_priv {
	sint fw_state;
	_queue scanned_queue;
	struct wlan_network cur_network;
	NDIS_802_11_SSID assoc_ssid;
	NDIS_802_11_MAC_ADDRESS assoc_bssid;
	u32 assoc_by_bssid;
	void *nic_hdl;
	_list *pscanned;
	struct wlan_network *roam_network;
};

struct _adapter {
	struct mlme_priv mlmepriv;
	struct rf_ctl_t rfctl;
	struct registry_priv registrypriv;
	struct dvobj_priv dvobj;
};

typedef struct _adapter _adapter;

#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#define adapter_to_rfctl(adapter) (&(adapter)->rfctl)
#define adapter_to_dvobj(adapter) (&(adapter)->dvobj)
#define LIST_CONTAINOR(ptr, type, member) container_of(ptr, type, member)
#define IS_DFS_SLAVE_WITH_RD(rfctl) 0
#define rtw_rfctl_dfs_domain_unknown(rfctl) 1
#define rtw_chset_is_ch_non_ocp(chset, ch) 0

static inline sint check_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
	if (state == 0 && pmlmepriv->fw_state == 0)
		return _TRUE;
	if (pmlmepriv->fw_state & state)
		return _TRUE;
	return _FALSE;
}

static inline void set_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
	pmlmepriv->fw_state |= state;
}

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
int rtw_chset_search_ch(RT_CHANNEL_INFO *ch_set, u32 ch);
int rtw_is_desired_network(_adapter *adapter, struct wlan_network *pnetwork);
int rtw_is_scan_deny(_adapter *adapter);
int rtw_check_join_candidate(struct mlme_priv *mlme,
			     struct wlan_network **candidate,
			     struct wlan_network *competitor);
int rtw_select_and_join_from_scanned_queue(struct mlme_priv *pmlmepriv);
u8 _rtw_sitesurvey_condition_check(const char *caller, _adapter *adapter, bool check_sc_interval);
int rtw_joinbss_cmd(_adapter *adapter, struct wlan_network *candidate);

#endif /* HOST_MLME_JOIN_SELECT_TYPES_H */
