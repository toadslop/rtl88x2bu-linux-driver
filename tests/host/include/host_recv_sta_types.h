/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RECV_STA_TYPES_H
#define HOST_RECV_STA_TYPES_H

#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 1
#define _FAIL 0
#define RTW_RX_HANDLED 2

#define ETH_ALEN 6
#define TID_NUM 16
#define BIT(x) (1U << (x))

#define WIFI_ASOC_STATE 0x00000001U
#define WIFI_UNDER_LINKING 0x00000080U

#define WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA 7

#define RTW_IEEE80211_FTYPE_DATA 0x0008

#define GetAddr1Ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 4))
#define get_addr2_ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 10))
#define GetAddr3Ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 16))
#define GetAddr4Ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 24))

#define MacAddr_isBcst(addr) \
	((addr)[0] == 0xff && (addr)[1] == 0xff && (addr)[2] == 0xff && \
	 (addr)[3] == 0xff && (addr)[4] == 0xff && (addr)[5] == 0xff)

#define is_broadcast_mac_addr(Addr) MacAddr_isBcst(Addr)

static inline int IS_MCAST(const u8 *da)
{
	return (da[0] & 0x01) ? _TRUE : _FALSE;
}

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define cpu_to_le16(x) (x)
#define le16_to_cpu(x) (x)
#else
static inline u16 cpu_to_le16(u16 x)
{
	return (u16)((x & 0xff) << 8 | (x >> 8));
}
static inline u16 le16_to_cpu(u16 x)
{
	return cpu_to_le16(x);
}
#endif

#define get_frame_sub_type(pbuf) \
	(le16_to_cpu(*(unsigned short *)(pbuf)) & \
	 (BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2)))

#define adapter_mac_addr(adapter) ((adapter)->mac_addr)
#define MLME_STATE(adapter) ((adapter)->mlmepriv.fw_state)

#define RTW_INFO(...) do { } while (0)
#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) (x)[0], (x)[1], (x)[2], (x)[3], (x)[4], (x)[5]
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(adapter) (adapter)

typedef u32 systime;
typedef int sint;

struct stainfo_stats {
	systime last_rx_time;
	u64 rx_data_pkts;
	u64 rx_data_bc_pkts;
	u64 rx_data_mc_pkts;
	u64 rx_data_qos_pkts[TID_NUM];
	u64 rx_bytes;
	u64 rx_bc_bytes;
	u64 rx_mc_bytes;
	u32 rxratecnt[128];
};

struct sta_info {
	struct stainfo_stats sta_stats;
};

struct sta_priv { u8 _pad; };

struct RT_LINK_DETECT_T {
	u32 NumRxOkInPeriod;
	u32 NumRxUnicastOkInPeriod;
};

struct wlan_bssid_ex {
	u8 MacAddress[ETH_ALEN];
};

struct wlan_network {
	struct wlan_bssid_ex network;
};

struct mlme_priv {
	u32 fw_state;
	struct wlan_network cur_network;
	struct RT_LINK_DETECT_T LinkDetectInfo;
};

struct recv_priv {
	u64 rx_bytes;
};

struct rf_ctl_t {
	u8 radar_detected; /* DFS stub for IS_RADAR_DETECTED */
};

struct rx_pkt_attrib {
	unsigned int len;
	u8 to_fr_ds;
	u8 amsdu;
	u8 priority;
	u8 data_rate;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 bssid[ETH_ALEN];
};

struct recv_frame_hdr {
	unsigned int len;
	u8 *rx_data;
	u8 *rx_tail;
	struct rx_pkt_attrib attrib;
	struct sta_info *psta;
};

union recv_frame {
	struct {
		struct recv_frame_hdr hdr;
	} u;
};

typedef struct {
	u8 mac_addr[ETH_ALEN];
	struct mlme_priv mlmepriv;
	struct recv_priv recvpriv;
	struct sta_priv stapriv;
	struct rf_ctl_t rfctl;
} _adapter;

static inline u8 *get_bssid(struct mlme_priv *pmlmepriv)
{
	return pmlmepriv->cur_network.network.MacAddress;
}

static inline int get_recvframe_len(union recv_frame *precvframe)
{
	return (int)precvframe->u.hdr.len;
}

static inline u8 *get_recvframe_data(union recv_frame *precvframe)
{
	return precvframe->u.hdr.rx_data;
}

static inline int host_rtw_memcmp(const void *a, const void *b, u32 sz)
{
	return memcmp(a, b, sz) == 0 ? _TRUE : _FALSE;
}

#define _rtw_memcmp host_rtw_memcmp

#define IS_RADAR_DETECTED(rf) ((rf)->radar_detected)

struct sta_info *rtw_get_stainfo(struct sta_priv *stapriv, u8 *hwaddr);
systime rtw_get_current_time(void);
s32 rtw_get_passing_time_ms(systime start);
void issue_deauth(_adapter *adapter, u8 *mac, u16 reason);

static inline struct rf_ctl_t *adapter_to_rfctl(_adapter *adapter)
{
	return &adapter->rfctl;
}

void count_rx_stats(_adapter *padapter, union recv_frame *prframe, struct sta_info *sta);
int rtw_sta_rx_data_validate_hdr(_adapter *adapter, union recv_frame *rframe, struct sta_info **sta);

void host_recv_sta_reset(void);
void host_recv_sta_register_sta(const u8 *mac, struct sta_info *sta);
u32 host_recv_sta_deauth_count(void);
void host_recv_sta_set_time(systime now, s32 passing_ms); /* test timing control */

#endif /* HOST_RECV_STA_TYPES_H */
