// SPDX-License-Identifier: GPL-2.0
/* Host L2 runner for WAPI SMS4 frame encrypt (W3-142 PR1). */
#include <stdio.h>
#include <string.h>

#include "host_wapi_sms4_frame_oracle.h"

static void setup_sta(struct host_wapi_adapter *adapter,
		      struct host_wapi_sta_info *sta, const u8 *peer_mac)
{
	static u8 key_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
				  0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
	static u8 pn[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01};

	memset(sta, 0, sizeof(*sta));
	memcpy(sta->peer_mac, peer_mac, HOST_WAPI_ETH_ALEN);
	memcpy(sta->usk.data_key, key_data, 16);
	sta->usk.key_id = 0;
	sta->usk.b_set = 1;
	memcpy(sta->last_tx_unicast_pn, pn, 16);
	adapter->wapi_info.sta_used_head = sta;
}

static int run_encrypt_changes_payload(void)
{
	struct host_wapi_adapter adapter;
	struct host_wapi_sta_info sta;
	struct host_wapi_xmit_frame xmit;
	u8 peer[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
	u8 plain[8] = {'H', 'e', 'l', 'l', 'o', '!', '!', '!'};
	u8 tx_buf[HOST_WAPI_FRAME_BUF];
	u8 before[8];
	u8 *wire;
	u16 hdrlen = HOST_WAPI_MAC_HDR;
	u16 iv_len = HOST_WAPI_EXT_LEN;
	int i;

	host_wapi_sms4_frame_adapter_init(&adapter);
	setup_sta(&adapter, &sta, peer);
	memset(&xmit, 0, sizeof(xmit));
	memset(tx_buf, 0, sizeof(tx_buf));
	wire = tx_buf + HOST_WAPI_TXDESC_SIZE;
	wire[0] = 0x08;
	wire[1] = 0x01;
	memcpy(wire + 4, peer, 6);
	wire[hdrlen] = 0;
	for (i = 0; i < 16; i++)
		wire[hdrlen + 2 + i] = sta.last_tx_unicast_pn[i];
	memcpy(wire + hdrlen + iv_len, plain, sizeof(plain));
	memcpy(before, wire + hdrlen + iv_len, sizeof(before));

	xmit.buf_addr = tx_buf;
	xmit.attrib.hdrlen = hdrlen;
	xmit.attrib.iv_len = (u8)iv_len;
	xmit.attrib.icv_len = HOST_WAPI_SMS4_MIC_LEN;
	xmit.attrib.pktlen = (u16)(sizeof(plain) + HOST_WAPI_SMS4_MIC_LEN);

	if (host_rtw_sms4_encrypt(&adapter, &xmit) != 1)
		return -1;
	if (memcmp(wire + hdrlen + iv_len, before, sizeof(before)) == 0)
		return -1;
	return 0;
}

static int run_wapi_disabled(void)
{
	struct host_wapi_adapter adapter;
	struct host_wapi_xmit_frame xmit;
	u8 buf[64];

	host_wapi_sms4_frame_adapter_init(&adapter);
	adapter.wapi_info.b_wapi_enable = 0;
	memset(&xmit, 0, sizeof(xmit));
	xmit.buf_addr = buf;
	return host_rtw_sms4_encrypt(&adapter, &xmit) == 1 ? -1 : 0;
}

int main(void)
{
	if (run_wapi_disabled() != 0 || run_encrypt_changes_payload() != 0) {
		fprintf(stderr, "wapi_sms4_frame encrypt tests failed\n");
		return 1;
	}
	printf("wapi_sms4_frame: ok\n");
	return 0;
}
