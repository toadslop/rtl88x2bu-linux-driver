/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_WAPI_SMS4_FRAME_ORACLE_H
#define HOST_WAPI_SMS4_FRAME_ORACLE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define HOST_WAPI_ETH_ALEN 6
#define HOST_WAPI_TXDESC_SIZE 48
#define HOST_WAPI_FRAME_BUF 512
#define HOST_WAPI_SMS4_MIC_LEN 16
#define HOST_WAPI_EXT_LEN 18
#define HOST_WAPI_MAC_HDR 24

struct host_wapi_key {
	u8 data_key[16];
	u8 key_id;
	u8 b_set;
};

struct host_wapi_sta_info {
	struct host_wapi_sta_info *next;
	u8 peer_mac[HOST_WAPI_ETH_ALEN];
	struct host_wapi_key usk;
	u8 last_tx_unicast_pn[16];
};

struct host_wapi_info {
	struct host_wapi_sta_info *sta_used_head;
	u8 b_wapi_enable;
	int extra_prefix_len;
	int extra_postfix_len;
};

struct host_wapi_adapter {
	u8 wapi_support;
	struct host_wapi_info wapi_info;
};

struct host_wapi_pkt_attrib {
	u16 hdrlen;
	u16 pktlen;
	u8 iv_len;
	u8 icv_len;
};

struct host_wapi_xmit_frame {
	u8 *buf_addr;
	struct host_wapi_pkt_attrib attrib;
};

void host_wapi_sms4_frame_adapter_init(struct host_wapi_adapter *a);
void host_sec_sw_sms4_encryption(struct host_wapi_adapter *padapter,
				 struct host_wapi_xmit_frame *pxmit);
u32 host_rtw_sms4_encrypt(struct host_wapi_adapter *padapter,
			  struct host_wapi_xmit_frame *pxmit);

#endif
