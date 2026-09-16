/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_BMC_UPDATE_TYPES_H
#define HOST_AP_BMC_UPDATE_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long _irqL;
typedef int _lock;

#define _TRUE 1
#define _FALSE 0
#define BAND_ON_5G 1
#define HOST_BMC_MAX_STA 4
#define HOST_BMC_MAX_RATES 12
#define WIFI_ASOC_STATE 0x00000001U
#define WIRELESS_11B (1U << 0)
#define WIRELESS_11BG (WIRELESS_11B | WIRELESS_11G)
#define WIRELESS_INVALID 0
#define MGN_UNKNOWN 0x00
#define MGN_1M 0x02
#define MGN_6M 0x0C
#define MGN_48M 0x60
#define MGN_12M 0x18
#define MGN_24M 0x30
#define WIFI_AP_STATE 0x00000010U
#define WIRELESS_11G (1U << 1)
#define WIRELESS_11A (1U << 2)
#define WIRELESS_11_24N (1U << 3)
#define WIRELESS_11_5N (1U << 4)
#define WIRELESS_11AC (1U << 6)
#define WIRELESS_MODE_5G (WIRELESS_11A | WIRELESS_11_5N | WIRELESS_11AC)
#define SUPPORTED_5G_NETTYPE_MSK WIRELESS_MODE_5G
#define IsSupportedTxCCK(NetType) (((NetType) & WIRELESS_11B) ? _TRUE : _FALSE)

#define IsEnableHWOFDM(NetType) \
	(((NetType) & (WIRELESS_11G | WIRELESS_11_24N | SUPPORTED_5G_NETTYPE_MSK)) ? \
	 _TRUE : \
	 _FALSE)

struct _list {
	struct _list *next;
	struct _list *prev;
};

struct host_ra_sta_info {
	u64 ramask;
	u8 curr_tx_rate;
};

struct host_cmn_sta_info {
	struct host_ra_sta_info ra_info;
};

struct host_stainfo_stats {
	u8 pad[8];
};

struct host_ht_priv {
	u8 ht_option;
};

struct sta_info {
	struct host_cmn_sta_info cmn;
	u16 aid;
	u8 qos_option;
	struct host_ht_priv htpriv;
	u8 ieee8021x_blocked;
	struct host_stainfo_stats sta_stats;
	_lock lock;
	u8 state;
	u8 wireless_mode;
	u8 bssrateset[HOST_BMC_MAX_RATES];
	u8 bssratelen;
	u8 init_rate;
	struct _list asoc_list;
};

struct sta_priv {
	struct _list asoc_list;
	_lock asoc_list_lock;
	int asoc_sta_count;
	struct sta_info *host_bcmc_sta;
};

struct host_wlan_bssid_ex {
	u8 SupportedRates[HOST_BMC_MAX_RATES];
	struct {
		u8 DSConfig;
	} Configuration;
};

struct mlme_priv {
	u32 state;
	struct {
		struct host_wlan_bssid_ex network;
	} cur_network;
};

struct mlme_ext_priv {
	u32 cur_wireless_mode;
};

typedef struct {
	u8 current_band_type;
} HAL_DATA_TYPE;

struct _adapter {
	HAL_DATA_TYPE hal_data;
	struct sta_priv stapriv;
	struct mlme_priv mlmepriv;
	struct mlme_ext_priv mlmeextpriv;
	u8 bmc_tx_rate;
};

#define GET_HAL_DATA(a) (&((a)->hal_data))
#define LIST_CONTAINOR(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))

static inline void _enter_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline void _exit_critical_bh(_lock *plock, _irqL *pirqL)
{
	(void)plock;
	(void)pirqL;
}

static inline struct _list *get_next(struct _list *list)
{
	return list->next;
}

static inline u8 rtw_end_of_queue_search(struct _list *queue, struct _list *pelement)
{
	return (queue == pelement) ? _TRUE : _FALSE;
}

#define MLME_IS_AP(a) (((a)->mlmepriv.state & WIFI_AP_STATE) != 0)
#define MLME_IS_MESH(a) 0

static inline void _rtw_init_listhead(struct _list *list)
{
	list->next = list;
	list->prev = list;
}

static inline void rtw_list_insert_tail(struct _list *n, struct _list *head)
{
	struct _list *prev = head->prev;

	n->next = head;
	n->prev = prev;
	prev->next = n;
	head->prev = n;
}

u8 rtw_ap_find_bmc_rate(struct _adapter *adapter, u8 tx_rate);
u8 rtw_ap_find_mini_tx_rate(struct _adapter *adapter);
struct sta_info *rtw_get_bcmc_stainfo(struct _adapter *padapter);
void rtw_init_bmc_sta_tx_rate(struct _adapter *padapter, struct sta_info *psta);
void rtw_update_bmc_sta_tx_rate(struct _adapter *adapter);
void update_bmc_sta(struct _adapter *padapter);

u32 rtw_get_rateset_len(u8 *rateset);
int rtw_check_network_type(u8 *rate, int ratelen, int channel);
void update_sta_basic_rate(struct sta_info *psta, u8 wireless_mode);
void rtw_hal_update_sta_ra_info(struct _adapter *padapter, struct sta_info *psta);
void rtw_sta_media_status_rpt(struct _adapter *padapter, struct sta_info *psta,
			      u8 connected);

void host_ap_bmc_sta_reset_hooks(void);
u8 host_ap_bmc_sta_media_rpt_count(void);

#endif
