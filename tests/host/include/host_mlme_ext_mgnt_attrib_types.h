/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Minimal types for host L2 mlme_ext mgnt attrib tests (W3-68).
 */
#ifndef HOST_MLME_EXT_MGNT_ATTRIB_TYPES_H
#define HOST_MLME_EXT_MGNT_ATTRIB_TYPES_H

#include "host_types.h"
#include "host_autoconf.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define BIT(n) (1U << (n))
#define BIT0 0x01
#define BIT1 0x02
#define ETH_ALEN 6

#define RTW_DEFAULT_MGMT_MACID 1
#define QSLT_MGNT 0x12
#define QSLT_VO 0x7
#define _NO_PRIVACY_ 0

#define WIRELESS_11B BIT0
#define WIRELESS_11G BIT1

#define IEEE80211_CCK_RATE_1MB 0x02
#define MGN_1M 0x02
#define MGN_MCS7 0x87
#define MGN_VHT1SS_MCS9 0x9f

#define IS_CCK_RATE(_rate) \
	((_rate) == MGN_1M || (_rate) == 0x04 || (_rate) == 0x0b || (_rate) == 0x16)

#define RATEID_IDX_B 8
#define RATEID_IDX_G 7
#define RATEID_IDX_VHT_1SS 10
#define RATEID_IDX_VHT_2SS 9
#define RATEID_IDX_VHT_3SS 13
#define RATEID_IDX_BGN_40M_1SS 1

#define WIFI_MGT_TYPE 0
#define WIFI_ACTION (BIT(7) | BIT(6) | BIT(4) | WIFI_MGT_TYPE)
#define WIFI_DISASSOC (BIT(7) | BIT(5) | WIFI_MGT_TYPE)
#define WIFI_DEAUTH (BIT(7) | BIT(6) | WIFI_MGT_TYPE)
#define WIFI_PROBERSP (BIT(6) | BIT(4) | WIFI_MGT_TYPE)
#define WIFI_BEACON (BIT(7) | WIFI_MGT_TYPE)

#define ACT_PUBLIC_FTM_REQ 14
#define ACT_PUBLIC_FTM 15

#define WIFI_ADHOC_STATE 0x00000004
#define CHK_MLME_STATE(adapter, state) \
	((((adapter)->host_fixture.mlme_state) & (state)) != 0)
#define MLME_IS_ADHOC(adapter) CHK_MLME_STATE((adapter), WIFI_ADHOC_STATE)

#define TXDESC_SIZE 40
#define PACKET_OFFSET_SZ 8
#define TXDESC_OFFSET (TXDESC_SIZE + PACKET_OFFSET_SZ)

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE 0

enum channel_width {
	CHANNEL_WIDTH_20 = 0,
	CHANNEL_WIDTH_40 = 1,
	CHANNEL_WIDTH_80 = 2,
};

enum rf_type {
	RF_1T1R = 0,
	RF_2T2R = 1,
	RF_2T4R = 2,
	RF_3T3R = 3,
};

#define RTW_ERR(...) do { } while (0)
#define RTW_INFO(...) do { } while (0)

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

#define GetAddr1Ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 4))
#define get_addr2_ptr(pbuf) ((u8 *)((uintptr_t)(pbuf) + 10))

struct bf_cmn_info {
	u8 ht_beamform_cap;
	u16 vht_beamform_cap;
	u16 p_aid;
	u8 g_id;
};

struct cmn_sta_info {
	u8 mac_id;
	struct bf_cmn_info bf_info;
};

struct sta_info {
	struct cmn_sta_info cmn;
};

struct sta_priv {
	struct sta_info *fixture_sta;
};

struct mlme_ext_priv {
	u16 mgnt_seq;
	u8 tx_rate;
};

struct xmit_priv {
	u8 hw_ssn_seq_no;
};

typedef struct {
	u8 rf_type;
} HAL_DATA_TYPE;

struct host_mgnt_attrib_fixture {
	u32 mlme_state;
};

struct _adapter {
	struct mlme_ext_priv mlmeextpriv;
	struct xmit_priv xmitpriv;
	struct sta_priv stapriv;
	HAL_DATA_TYPE hal_data;
	struct host_mgnt_attrib_fixture host_fixture;
};

typedef struct _adapter _adapter;

struct pkt_attrib {
	u8 type;
	u8 subtype;
	u8 bswenc;
	u8 dhcp_pkt;
	u16 ether_type;
	u16 seqnum;
	u8 hw_ssn_sel;
	u16 pkt_hdrlen;
	u16 hdrlen;
	u32 pktlen;
	u32 last_txcmdsz;
	u8 nr_frags;
	u8 encrypt;
	u8 bmc_camid;
	u8 iv_len;
	u8 icv_len;
	u8 priority;
	u8 ack_policy;
	u8 mac_id;
	u8 vcs_mode;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
	u8 ta[ETH_ALEN];
	u8 ra[ETH_ALEN];
	u8 key_idx;
	u8 qos_en;
	u8 ht_en;
	u8 raid;
	u8 bwmode;
	u8 ch_offset;
	u8 sgi;
	u8 mdata;
	u8 order;
	u8 rate;
	u8 retry_ctrl;
	u8 mbssid;
	u8 qsel;
	struct sta_info *psta;
#ifdef CONFIG_BEAMFORMING
	u16 txbf_p_aid;
	u16 txbf_g_id;
#endif
#ifdef CONFIG_RTW_MGMT_QUEUE
	u8 ps_dontq;
#endif
};

struct xmit_frame {
	struct pkt_attrib attrib;
	u8 *buf_addr;
};

#define GET_HAL_DATA(adapter) (&((adapter)->hal_data))

u8 rtw_get_mgntframe_raid(_adapter *adapter, unsigned char network_type);
struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, const u8 *hwaddr);
int rtw_action_frame_parse(const u8 *frame, u32 frame_len, u8 *category,
			   u8 *action);
void update_attrib_txbf_info(_adapter *padapter, struct pkt_attrib *pattrib,
			     struct sta_info *psta);

void update_monitor_frame_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
#ifdef CONFIG_RTW_MGMT_QUEUE
void update_mgntframe_subtype(_adapter *padapter, struct xmit_frame *pmgntframe);
#endif
void update_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib);
void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe);

extern _adapter *host_mgnt_attrib_adapter;

#endif /* HOST_MLME_EXT_MGNT_ATTRIB_TYPES_H */
