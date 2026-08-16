/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal userspace types for host L2 recv_rest tests (W3-39, W3-46, W3-47).
 */
#ifndef HOST_RECV_TYPES_H
#define HOST_RECV_TYPES_H

#include <stdbool.h>
#include <string.h>

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define _SUCCESS 0
#define _FAIL (-1)
#define TID_NUM 16
#define MAX_CONTINUAL_NORXPACKET_COUNT 4
#define ETH_ALEN 6
#define WLAN_EID_VENDOR_SPECIFIC 221
#define MAX_IE_SZ 768
#define HOST_RECV_MAX_FRAME 512

#define WIFI_ASOC_STATE 0x00000001
#define WIFI_STATION_STATE 0x00000008
#define WIFI_AP_STATE 0x00000010
#define WIFI_MP_STATE 0x00010000
#define WIFI_MONITOR_STATE 0x80000000

#define ETH_P_AARP 0x80F3
#define ETH_P_IPX 0x8137

#define RTW_GET_BE16(a) ((u16)(((a)[0] << 8) | (a)[1]))

#define _AES_ 0x04
#define CCMPH_2_PN(ch) \
	(((ch) & 0x000000000000ffffULL) | (((ch) & 0xffffffff00000000ULL) >> 16))
#define CCMPH_2_KEYID(ch) (((ch) & 0x00000000c0000000ULL) >> 30)

typedef int ATOMIC_T;
typedef int sint;
#define ATOMIC_INC_RETURN(v) __sync_add_and_fetch((v), 1)
#define ATOMIC_SET(v, x) (*(v) = (x))

static inline u64 host_le64_to_cpu(u64 v)
{
	return v;
}

static inline u64 host_cpu_to_le64(u64 v)
{
	return v;
}

static inline sint host_is_mcast(const u8 *da)
{
	return (da[0] & 0x01) ? _TRUE : _FALSE;
}

struct _adapter;

struct sta_stats_host {
	u32 duplicate_cnt;
};

struct stainfo_rxcache {
	u16 tid_rxseq[TID_NUM];
	u8 iv[TID_NUM][8];
	u8 last_tid;
};

struct sta_recv_priv {
	struct stainfo_rxcache rxcache;
	u16 bmc_tid_rxseq[TID_NUM];
	u16 nonqos_rxseq;
	u16 nonqos_bmc_rxseq;
};

struct sta_info {
	ATOMIC_T continual_no_rx_packet[TID_NUM];
	struct _adapter *padapter;
	struct sta_recv_priv sta_recvpriv;
	struct sta_stats_host sta_stats;
};

struct rtw_ieee80211_hdr_3addr {
	u16 frame_ctl;
	u16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	u16 seq_ctl;
} __attribute__((packed));

struct ieee80211_snap_hdr {
	u8 dsap;
	u8 ssap;
	u8 ctrl;
	u8 oui[3];
} __attribute__((packed));

#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)

struct ethhdr {
	u8 h_dest[ETH_ALEN];
	u8 h_source[ETH_ALEN];
	u16 h_proto;
} __attribute__((packed));

enum rtw_rx_llc_hdl {
	RTW_RX_LLC_KEEP = 0,
	RTW_RX_LLC_REMOVE = 1,
	RTW_RX_LLC_VLAN = 2,
};

struct rx_pkt_attrib {
	u8 hdrlen;
	u8 encrypt;
	u8 iv_len;
	u8 icv_len;
	u8 qos;
	u8 priority;
	u16 seq_num;
	u8 frag_num;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ra[ETH_ALEN];
};

#define RATTRIB_GET_MCTRL_LEN(rattrib) 0

struct mlme_priv {
	sint fw_state;
};

struct security_priv {
	u8 iv_seq[4][8];
};

struct _adapter {
	struct mlme_priv mlmepriv;
	u8 host_linked;
	struct security_priv securitypriv;
};

typedef struct _adapter _adapter;

struct recv_frame_hdr {
	u32 _pad;
	unsigned int len;
	u8 *rx_head;
	u8 *rx_data;
	u8 *rx_tail;
	u8 *rx_end;
	struct rx_pkt_attrib attrib;
	_adapter *adapter;
	struct sta_info *psta;
};

union recv_frame {
	struct {
		struct recv_frame_hdr hdr;
	} u;
};

extern u8 rtw_bridge_tunnel_header[];
extern u8 rtw_rfc1042_header[];

static inline int host_rtw_memcmp(const void *dst, const void *src, u32 sz)
{
	return memcmp(dst, src, sz) == 0 ? _TRUE : _FALSE;
}

static inline sint check_fwstate(struct mlme_priv *pmlmepriv, sint state)
{
	if (state == 0 && pmlmepriv->fw_state == 0)
		return _TRUE;
	if (pmlmepriv->fw_state & state)
		return _TRUE;
	return _FALSE;
}

#define MLME_IS_AP(adapter) check_fwstate(&((adapter)->mlmepriv), WIFI_AP_STATE)

static inline u16 host_htons(u16 x)
{
	return (u16)(((x & 0xff) << 8) | ((x >> 8) & 0xff));
}

#define htons host_htons

static inline u8 *get_recvframe_data(union recv_frame *precvframe)
{
	if (!precvframe)
		return NULL;
	return precvframe->u.hdr.rx_data;
}

static inline u8 *recvframe_pull(union recv_frame *precvframe, sint sz)
{
	if (!precvframe)
		return NULL;
	precvframe->u.hdr.rx_data += sz;
	if (precvframe->u.hdr.rx_data > precvframe->u.hdr.rx_tail) {
		precvframe->u.hdr.rx_data -= sz;
		return NULL;
	}
	precvframe->u.hdr.len -= (unsigned int)sz;
	return precvframe->u.hdr.rx_data;
}

static inline u8 *recvframe_pull_tail(union recv_frame *precvframe, sint sz)
{
	if (!precvframe)
		return NULL;
	precvframe->u.hdr.rx_tail -= sz;
	if (precvframe->u.hdr.rx_tail < precvframe->u.hdr.rx_data)
		return NULL;
	precvframe->u.hdr.len -= (unsigned int)sz;
	return precvframe->u.hdr.rx_tail;
}

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);
bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset);
u8 *rtw_get_wfd_ie(const u8 *in_ie, int in_len, u8 *wfd_ie, unsigned int *wfd_ielen);
unsigned int rtw_del_wfd_ie(u8 *ies, unsigned int ies_len_ori, const char *msg);

enum rtw_rx_llc_hdl rtw_recv_llc_parse(u8 *msdu, u16 msdu_len);
sint wlanhdr_to_ethhdr(union recv_frame *precvframe, enum rtw_rx_llc_hdl llc_hdl);
u8 adapter_allow_bmc_data_rx(_adapter *adapter);
void rtw_rframe_set_os_pkt(union recv_frame *rframe);
sint rtw_linked_check(_adapter *adapter);

sint recv_decache(union recv_frame *precv_frame);
sint recv_ucast_pn_decache(union recv_frame *precv_frame);
sint recv_bcast_pn_decache(union recv_frame *precv_frame);

#endif /* HOST_RECV_TYPES_H */
