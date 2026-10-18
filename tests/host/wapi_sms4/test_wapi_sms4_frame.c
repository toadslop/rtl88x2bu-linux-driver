// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for WAPI SMS4 frame encrypt/decrypt (W3-142 PR2). */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_frame_oracle.h"

static void setup_sta(struct host_wapi_adapter *adapter,
		      struct host_wapi_sta_info *sta, const u8 *peer_mac)
{
	static u8 key_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
				  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
	static u8 pn[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1};

	memset(sta, 0, sizeof(*sta));
	memcpy(sta->peer_mac, peer_mac, HOST_WAPI_ETH_ALEN);
	memcpy(sta->usk.data_key, key_data, 16);
	sta->usk.b_set = 1;
	memcpy(sta->last_tx_unicast_pn, pn, 16);
	adapter->wapi_info.sta_used_head = sta;
}

static int run_roundtrip(void)
{
	struct host_wapi_adapter adapter;
	struct host_wapi_sta_info sta;
	struct host_wapi_xmit_frame xmit;
	union host_wapi_recv_frame recv;
	struct host_wapi_skb skb;
	u8 peer[6] = {0x02, 0, 0, 0, 0, 1};
	u8 plain[8] = "Hello!!!";
	u8 tx_buf[HOST_WAPI_FRAME_BUF];
	u8 rx_buf[HOST_WAPI_FRAME_BUF];
	u8 *wire;
	u16 hdrlen = HOST_WAPI_MAC_HDR;
	u16 iv_len = HOST_WAPI_EXT_LEN;
	int i;

	host_wapi_sms4_frame_adapter_init(&adapter);
	setup_sta(&adapter, &sta, peer);
	memset(&xmit, 0, sizeof(xmit));
	wire = tx_buf + HOST_WAPI_TXDESC_SIZE;
	xmit.buf_addr = tx_buf;
	wire[0] = 0x08;
	memcpy(wire + 4, peer, 6);
	memcpy(wire + 10, peer, 6);
	for (i = 0; i < 16; i++)
		wire[hdrlen + 2 + i] = sta.last_tx_unicast_pn[i];
	memcpy(wire + hdrlen + iv_len, plain, sizeof(plain));
	xmit.attrib.hdrlen = hdrlen;
	xmit.attrib.iv_len = (u8)iv_len;
	xmit.attrib.icv_len = HOST_WAPI_SMS4_MIC_LEN;
	xmit.attrib.pktlen = (u16)(sizeof(plain) + HOST_WAPI_SMS4_MIC_LEN);
	if (host_rtw_sms4_encrypt(&adapter, &xmit) != 1)
		return -1;

	memcpy(rx_buf, wire, hdrlen + iv_len + xmit.attrib.pktlen);
	skb.data = rx_buf;
	skb.len = hdrlen + iv_len + xmit.attrib.pktlen;
	recv.hdr.rx_skb = &skb;
	if (host_rtw_sms4_decrypt(&adapter, &recv) != 1)
		return -1;
	if (skb.len < hdrlen + sizeof(plain) ||
	    memcmp(skb.data + hdrlen, plain, sizeof(plain)) != 0)
		return -1;
	return 0;
}

int main(void)
{
	if (run_roundtrip() != 0) {
		fprintf(stderr, "wapi_sms4_frame roundtrip failed\n");
		return 1;
	}
	printf("wapi_sms4_frame: ok\n");
	return 0;
}
