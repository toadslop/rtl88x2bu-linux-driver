// SPDX-License-Identifier: GPL-2.0
/* C oracle for WAPI SMS4 frame encrypt (W3-142 PR1). */
#include <string.h>

#include "host_wapi_sms4_frame_oracle.h"
#include "host_wapi_sms4_ofb_oracle.h"

#define _SUCCESS 1
#define _FAIL 0

static struct host_wapi_sta_info *find_sta(struct host_wapi_info *info,
					   const u8 *mac)
{
	struct host_wapi_sta_info *sta = info->sta_used_head;

	while (sta) {
		if (memcmp(sta->peer_mac, mac, HOST_WAPI_ETH_ALEN) == 0)
			return sta;
		sta = sta->next;
	}
	return NULL;
}

void host_wapi_sms4_frame_adapter_init(struct host_wapi_adapter *a)
{
	memset(a, 0, sizeof(*a));
	a->wapi_support = 1;
	a->wapi_info.b_wapi_enable = 1;
	a->wapi_info.extra_prefix_len = HOST_WAPI_EXT_LEN;
	a->wapi_info.extra_postfix_len = HOST_WAPI_SMS4_MIC_LEN;
}

void host_sec_sw_sms4_encryption(struct host_wapi_adapter *padapter,
				 struct host_wapi_xmit_frame *pxmit)
{
	struct host_wapi_info *info = &padapter->wapi_info;
	u8 *pframe = pxmit->buf_addr + HOST_WAPI_TXDESC_SIZE;
	struct host_wapi_pkt_attrib *pattrib = &pxmit->attrib;
	u8 *pRA, *pDataKey, *pIV;
	u8 data_offset;
	u16 output_length, crypt_len;
	struct host_wapi_sta_info *sta;

	data_offset = (u8)(pattrib->hdrlen + pattrib->iv_len);
	pRA = pframe + 4;
	sta = find_sta(info, pRA);
	if (!sta)
		return;

	pIV = sta->last_tx_unicast_pn;
	pDataKey = sta->usk.data_key;

	/* Match SecSWSMS4Encryption → WapiSMS4Encryption length (MIC calc is #if 0 in harness). */
	crypt_len = pattrib->pktlen + pattrib->icv_len;
	host_wapi_sms4_encryption(pDataKey, pIV, pframe + data_offset, crypt_len,
				pframe + data_offset, &output_length);
}

u8 host_sec_sw_sms4_decryption(struct host_wapi_adapter *padapter,
			       union host_wapi_recv_frame *precv)
{
	struct host_wapi_info *info = &padapter->wapi_info;
	struct host_wapi_skb *pskb = precv->hdr.rx_skb;
	u8 iv_offset = HOST_WAPI_MAC_HDR;
	u8 data_offset;
	u8 *pRA, *pTA, *pDataKey, *pRecvPN, *pSecData;
	u16 output_length, payload_len;
	struct host_wapi_sta_info *sta;

	data_offset = iv_offset + info->extra_prefix_len;
	pRA = pskb->data + 4;
	pTA = pskb->data + 10;
	pRecvPN = pskb->data + iv_offset + 2;
	pSecData = pskb->data + data_offset;
	payload_len = (u16)(pskb->len - data_offset - info->extra_postfix_len);
	sta = find_sta(info, pTA);
	if (!sta || (pRA[0] & 1))
		return 0;
	pDataKey = sta->usk.data_key;
	host_wapi_sms4_decryption(pDataKey, pRecvPN, pSecData, payload_len,
				  pSecData, &output_length);
	memmove(pskb->data + info->extra_prefix_len, pskb->data, iv_offset);
	pskb->data += info->extra_prefix_len;
	pskb->len -= info->extra_prefix_len;
	return 1;
}

u32 host_rtw_sms4_encrypt(struct host_wapi_adapter *padapter,
			  struct host_wapi_xmit_frame *pxmit)
{
	if (!padapter->wapi_support || !padapter->wapi_info.b_wapi_enable)
		return _FAIL;
	if (!pxmit->buf_addr)
		return _FAIL;
	host_sec_sw_sms4_encryption(padapter, pxmit);
	return _SUCCESS;
}

u32 host_rtw_sms4_decrypt(struct host_wapi_adapter *padapter,
			  union host_wapi_recv_frame *precv)
{
	if (!padapter->wapi_support || !padapter->wapi_info.b_wapi_enable)
		return _FAIL;
	if (!host_sec_sw_sms4_decryption(padapter, precv))
		return _FAIL;
	return _SUCCESS;
}

#ifndef WAPI_SMS4_FRAME_L1_REF
/* Production symbol names for Rust L1 / differential runner */
void SecSWSMS4Encryption(void *padapter, u8 *pxmitframe)
{
	host_sec_sw_sms4_encryption((struct host_wapi_adapter *)padapter,
				    (struct host_wapi_xmit_frame *)pxmitframe);
}

u32 rtw_sms4_encrypt(void *padapter, u8 *pxmitframe)
{
	return host_rtw_sms4_encrypt((struct host_wapi_adapter *)padapter,
				     (struct host_wapi_xmit_frame *)pxmitframe);
}

u8 SecSWSMS4Decryption(void *padapter, u8 *precv_frame, void *precv_priv)
{
	(void)precv_priv;
	return host_sec_sw_sms4_decryption((struct host_wapi_adapter *)padapter,
					   (union host_wapi_recv_frame *)precv_frame);
}

u32 rtw_sms4_decrypt(void *padapter, u8 *precvframe)
{
	return host_rtw_sms4_decrypt((struct host_wapi_adapter *)padapter,
				     (union host_wapi_recv_frame *)precvframe);
}

#else

void SecSWSMS4Encryption(void *padapter, u8 *pxmitframe)
{
	host_sec_sw_sms4_encryption((struct host_wapi_adapter *)padapter,
				    (struct host_wapi_xmit_frame *)pxmitframe);
}

u32 rtw_sms4_encrypt(void *padapter, u8 *pxmitframe)
{
	return host_rtw_sms4_encrypt((struct host_wapi_adapter *)padapter,
				     (struct host_wapi_xmit_frame *)pxmitframe);
}

u8 SecSWSMS4Decryption(void *padapter, u8 *precv_frame, void *precv_priv)
{
	(void)precv_priv;
	return host_sec_sw_sms4_decryption((struct host_wapi_adapter *)padapter,
					   (union host_wapi_recv_frame *)precv_frame);
}

u32 rtw_sms4_decrypt(void *padapter, u8 *precvframe)
{
	return host_rtw_sms4_decrypt((struct host_wapi_adapter *)padapter,
				     (union host_wapi_recv_frame *)precvframe);
}

#endif
