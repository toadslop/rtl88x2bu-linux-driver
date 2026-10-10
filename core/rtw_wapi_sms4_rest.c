/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_WAPI_SMS4_REST_C_

#ifdef CONFIG_WAPI_SUPPORT

#include <linux/unistd.h>
#include <linux/etherdevice.h>
#include <drv_types.h>
#include <rtw_wapi.h>

#ifdef CONFIG_WAPI_SW_SMS4

#define ENCRYPT  0
#define DECRYPT  1

void xor_block(void *dst, void *src1, void *src2);
void SMS4Crypt(u8 *Input, u8 *Output, u32 *rk);
void SMS4KeyExt(u8 *Key, u32 *rk, u32 CryptFlag);
void WapiSMS4Encryption(u8 *Key, u8 *IV, u8 *Input, u16 InputLength,
			u8 *Output, u16 *OutputLength);
void WapiSMS4Decryption(u8 *Key, u8 *IV, u8 *Input, u16 InputLength,
			u8 *Output, u16 *OutputLength);

/* WapiSMS4Cryption / WapiSMS4Encryption / WapiSMS4Decryption: rust/rtw_wapi_sms4.rs (W3-139) */

void WapiSMS4CalculateMic(u8 *Key, u8 *IV, u8 *Input1, u8 Input1Length,
		  u8 *Input2, u16 Input2Length, u8 *Output, u8 *OutputLength)
{
	u32 blockNum, i, remainder, rk[32];
	u8 BlockIn[16], BlockOut[16], TempBlock[16], tempIV[16], k;

	*OutputLength = 0;
	remainder = Input1Length & 0x0F;
	blockNum = Input1Length >> 4;

	for (k = 0; k < 16; k++)
		tempIV[k] = IV[15 - k];

	memcpy(BlockIn, tempIV, 16);

	SMS4KeyExt((u8 *)Key, rk, ENCRYPT);

	SMS4Crypt((u8 *)BlockIn, BlockOut, rk);

	for (i = 0; i < blockNum; i++) {
		xor_block(BlockIn, (Input1 + i * 16), BlockOut);
		SMS4Crypt((u8 *)BlockIn, BlockOut, rk);
	}

	if (remainder != 0) {
		memset(TempBlock, 0, 16);
		memcpy(TempBlock, (Input1 + blockNum * 16), remainder);

		xor_block(BlockIn, TempBlock, BlockOut);
		SMS4Crypt((u8 *)BlockIn, BlockOut, rk);
	}

	remainder = Input2Length & 0x0F;
	blockNum = Input2Length >> 4;

	for (i = 0; i < blockNum; i++) {
		xor_block(BlockIn, (Input2 + i * 16), BlockOut);
		SMS4Crypt((u8 *)BlockIn, BlockOut, rk);
	}

	if (remainder != 0) {
		memset(TempBlock, 0, 16);
		memcpy(TempBlock, (Input2 + blockNum * 16), remainder);

		xor_block(BlockIn, TempBlock, BlockOut);
		SMS4Crypt((u8 *)BlockIn, BlockOut, rk);
	}

	memcpy(Output, BlockOut, 16);
	*OutputLength = 16;
}

void SecCalculateMicSMS4(
	u8		KeyIdx,
	u8        *MicKey,
	u8        *pHeader,
	u8        *pData,
	u16       DataLen,
	u8        *MicBuffer
)
{
#if 0
	struct ieee80211_hdr_3addr_qos *header;
	u8 TempBuf[34], TempLen = 32, MicLen, QosOffset, *IV;
	u16 *pTemp, fc;

	WAPI_TRACE(WAPI_TX | WAPI_RX, "=========>%s\n", __FUNCTION__);

	header = (struct ieee80211_hdr_3addr_qos *)pHeader;
	memset(TempBuf, 0, 34);
	memcpy(TempBuf, pHeader, 2); /* FrameCtrl */
	pTemp = (u16 *)TempBuf;
	*pTemp &= 0xc78f;       /* bit4,5,6,11,12,13 */

	memcpy((TempBuf + 2), (pHeader + 4), 12); /* Addr1, Addr2 */
	memcpy((TempBuf + 14), (pHeader + 22), 2); /* SeqCtrl */
	pTemp = (u16 *)(TempBuf + 14);
	*pTemp &= 0x000f;

	memcpy((TempBuf + 16), (pHeader + 16), 6); /* Addr3 */

	fc = le16_to_cpu(header->frame_ctl);



	if (GetFrDs((u16 *)&fc) && GetToDs((u16 *)&fc)) {
		memcpy((TempBuf + 22), (pHeader + 24), 6);
		QosOffset = 30;
	} else {
		memset((TempBuf + 22), 0, 6);
		QosOffset = 24;
	}

	if ((fc & 0x0088) == 0x0088) {
		memcpy((TempBuf + 28), (pHeader + QosOffset), 2);
		TempLen += 2;
		/* IV = pHeader + QosOffset + 2 + SNAP_SIZE + sizeof(u16) + 2; */
		IV = pHeader + QosOffset + 2 + 2;
	} else {
		IV = pHeader + QosOffset + 2;
		/* IV = pHeader + QosOffset + SNAP_SIZE + sizeof(u16) + 2; */
	}

	TempBuf[TempLen - 1] = (u8)(DataLen & 0xff);
	TempBuf[TempLen - 2] = (u8)((DataLen & 0xff00) >> 8);
	TempBuf[TempLen - 4] = KeyIdx;

	WAPI_DATA(WAPI_TX, "CalculateMic - KEY", MicKey, 16);
	WAPI_DATA(WAPI_TX, "CalculateMic - IV", IV, 16);
	WAPI_DATA(WAPI_TX, "CalculateMic - TempBuf", TempBuf, TempLen);
	WAPI_DATA(WAPI_TX, "CalculateMic - pData", pData, DataLen);

	WapiSMS4CalculateMic(MicKey, IV, TempBuf, TempLen,
			     pData, DataLen, MicBuffer, &MicLen);

	if (MicLen != 16)
		WAPI_TRACE(WAPI_ERR, "%s: MIC Length Error!!\n", __FUNCTION__);

	WAPI_TRACE(WAPI_TX | WAPI_RX, "<=========%s\n", __FUNCTION__);
#endif
}

/* WapiIncreasePN: rust/rtw_wapi_sms4.rs (W3-136) */

void WapiGetLastRxUnicastPNForQoSData(u8 UserPriority, PRT_WAPI_STA_INFO pWapiStaInfo,
				      u8 *PNOut);
void WapiSetLastRxUnicastPNForQoSData(u8 UserPriority, u8 *PNIn,
				      PRT_WAPI_STA_INFO pWapiStaInfo);
u8 WapiCheckPnInSwDecrypt(_adapter *padapter, struct sk_buff *pskb);

/* WapiGet/SetLastRxUnicastPNForQoSData + WapiCheckPnInSwDecrypt: rust/rtw_wapi_sms4.rs (W3-141) */

/* SecSMS4HeaderFillIV: rust/rtw_wapi_sms4.rs (W3-136) */

/* WAPI SW Enc: must have done Coalesce! */
void SecSWSMS4Encryption(
	_adapter *padapter,
	u8 *pxmitframe
)
{
	PRT_WAPI_T		pWapiInfo = &padapter->wapiInfo;
	PRT_WAPI_STA_INFO   pWapiSta = NULL;
	u8 *pframe = ((struct xmit_frame *)pxmitframe)->buf_addr + TXDESC_SIZE;
	struct pkt_attrib *pattrib = &((struct xmit_frame *)pxmitframe)->attrib;

	u8 *SecPtr = NULL, *pRA, *pMicKey = NULL, *pDataKey = NULL, *pIV = NULL;
	u8 IVOffset, DataOffset, bFindMatchPeer = false, KeyIdx = 0, MicBuffer[16];
	u16 OutputLength;

	WAPI_TRACE(WAPI_TX, "=========>%s\n", __FUNCTION__);

	WAPI_TRACE(WAPI_TX, "hdrlen: %d\n", pattrib->hdrlen);

	return;

	DataOffset = pattrib->hdrlen + pattrib->iv_len;

	pRA = pframe + 4;


	if (IS_MCAST(pRA)) {
		KeyIdx = pWapiInfo->wapiTxMsk.keyId;
		pIV = pWapiInfo->lastTxMulticastPN;
		pMicKey = pWapiInfo->wapiTxMsk.micKey;
		pDataKey = pWapiInfo->wapiTxMsk.dataKey;
	} else {
		if (!list_empty(&(pWapiInfo->wapiSTAUsedList))) {
			list_for_each_entry(pWapiSta, &pWapiInfo->wapiSTAUsedList, list) {
				if (0 == memcmp(pWapiSta->PeerMacAddr, pRA, 6)) {
					bFindMatchPeer = true;
					break;
				}
			}

			if (bFindMatchPeer) {
				if (pWapiSta->wapiUskUpdate.bTxEnable) {
					KeyIdx = pWapiSta->wapiUskUpdate.keyId;
					WAPI_TRACE(WAPI_TX, "%s(): Use update USK!! KeyIdx=%d\n", __FUNCTION__, KeyIdx);
					pIV = pWapiSta->lastTxUnicastPN;
					pMicKey = pWapiSta->wapiUskUpdate.micKey;
					pDataKey = pWapiSta->wapiUskUpdate.dataKey;
				} else {
					KeyIdx = pWapiSta->wapiUsk.keyId;
					WAPI_TRACE(WAPI_TX, "%s(): Use USK!! KeyIdx=%d\n", __FUNCTION__, KeyIdx);
					pIV = pWapiSta->lastTxUnicastPN;
					pMicKey = pWapiSta->wapiUsk.micKey;
					pDataKey = pWapiSta->wapiUsk.dataKey;
				}
			} else {
				WAPI_TRACE(WAPI_ERR, "%s: Can not find Peer Sta!!\n", __FUNCTION__);
				return;
			}
		} else {
			WAPI_TRACE(WAPI_ERR, "%s: wapiSTAUsedList is empty!!\n", __FUNCTION__);
			return;
		}
	}

	SecPtr = pframe;
	SecCalculateMicSMS4(KeyIdx, pMicKey, SecPtr, (SecPtr + DataOffset), pattrib->pktlen, MicBuffer);

	WAPI_DATA(WAPI_TX, "Encryption - MIC", MicBuffer, padapter->wapiInfo.extra_postfix_len);

	memcpy(pframe + pattrib->hdrlen + pattrib->iv_len + pattrib->pktlen - pattrib->icv_len,
	       (u8 *)MicBuffer,
	       padapter->wapiInfo.extra_postfix_len
	      );


	WapiSMS4Encryption(pDataKey, pIV, (SecPtr + DataOffset), pattrib->pktlen + pattrib->icv_len, (SecPtr + DataOffset), &OutputLength);

	WAPI_DATA(WAPI_TX, "Encryption - After SMS4 encryption", pframe, pattrib->hdrlen + pattrib->iv_len + pattrib->pktlen);

	WAPI_TRACE(WAPI_TX, "<=========%s\n", __FUNCTION__);
}

u8 SecSWSMS4Decryption(
	_adapter *padapter,
	u8		*precv_frame,
	struct recv_priv *precv_priv
)
{
	PRT_WAPI_T pWapiInfo = &padapter->wapiInfo;
	struct recv_frame_hdr *precv_hdr;
	PRT_WAPI_STA_INFO   pWapiSta = NULL;
	u8 IVOffset, DataOffset, bFindMatchPeer = false, bUseUpdatedKey = false;
	u8 KeyIdx, MicBuffer[16], lastRxPNforQoS[16];
	u8 *pRA, *pTA, *pMicKey, *pDataKey, *pLastRxPN, *pRecvPN, *pSecData, *pRecvMic, *pos;
	u8 TID = 0;
	u16 OutputLength, DataLen;
	u8   bQosData;
	struct sk_buff	*pskb;

	WAPI_TRACE(WAPI_RX, "=========>%s\n", __FUNCTION__);

	return 0;

	precv_hdr = &((union recv_frame *)precv_frame)->u.hdr;
	pskb = (struct sk_buff *)(precv_hdr->rx_data);
	precv_hdr->bWapiCheckPNInDecrypt = WapiCheckPnInSwDecrypt(padapter, pskb);
	WAPI_TRACE(WAPI_RX, "=========>%s: check PN  %d\n", __FUNCTION__, precv_hdr->bWapiCheckPNInDecrypt);
	WAPI_DATA(WAPI_RX, "Decryption - Before decryption", pskb->data, pskb->len);

	IVOffset = sMacHdrLng;
	bQosData = GetFrameType(pskb->data) == WIFI_QOS_DATA_TYPE;
	if (bQosData)
		IVOffset += 2;

	/* if(GetHTC()) */
	/*	IVOffset += 4; */

	/* IVOffset += SNAP_SIZE + sizeof(u16); */

	DataOffset = IVOffset + padapter->wapiInfo.extra_prefix_len;

	pRA = pskb->data + 4;
	pTA = pskb->data + 10;
	KeyIdx = *(pskb->data + IVOffset);
	pRecvPN = pskb->data + IVOffset + 2;
	pSecData = pskb->data + DataOffset;
	DataLen = pskb->len - DataOffset;
	pRecvMic = pskb->data + pskb->len - padapter->wapiInfo.extra_postfix_len;
	TID = GetTid(pskb->data);

	if (!list_empty(&(pWapiInfo->wapiSTAUsedList))) {
		list_for_each_entry(pWapiSta, &pWapiInfo->wapiSTAUsedList, list) {
			if (0 == memcmp(pWapiSta->PeerMacAddr, pTA, 6)) {
				bFindMatchPeer = true;
				break;
			}
		}
	}

	if (!bFindMatchPeer) {
		WAPI_TRACE(WAPI_ERR, "%s: Can not find Peer Sta "MAC_FMT" for Key Info!!!\n", __FUNCTION__, MAC_ARG(pTA));
		return false;
	}

	if (IS_MCAST(pRA)) {
		WAPI_TRACE(WAPI_RX, "%s: Multicast decryption !!!\n", __FUNCTION__);
		if (pWapiSta->wapiMsk.keyId == KeyIdx && pWapiSta->wapiMsk.bSet) {
			pLastRxPN = pWapiSta->lastRxMulticastPN;
			if (!WapiComparePN(pRecvPN, pLastRxPN)) {
				WAPI_TRACE(WAPI_ERR, "%s: MSK PN is not larger than last, Dropped!!!\n", __FUNCTION__);
				WAPI_DATA(WAPI_ERR, "pRecvPN:", pRecvPN, 16);
				WAPI_DATA(WAPI_ERR, "pLastRxPN:", pLastRxPN, 16);
				return false;
			}

			memcpy(pLastRxPN, pRecvPN, 16);
			pMicKey = pWapiSta->wapiMsk.micKey;
			pDataKey = pWapiSta->wapiMsk.dataKey;
		} else if (pWapiSta->wapiMskUpdate.keyId == KeyIdx && pWapiSta->wapiMskUpdate.bSet) {
			WAPI_TRACE(WAPI_RX, "%s: Use Updated MSK for Decryption !!!\n", __FUNCTION__);
			bUseUpdatedKey = true;
			memcpy(pWapiSta->lastRxMulticastPN, pRecvPN, 16);
			pMicKey = pWapiSta->wapiMskUpdate.micKey;
			pDataKey = pWapiSta->wapiMskUpdate.dataKey;
		} else {
			WAPI_TRACE(WAPI_ERR, "%s: Can not find MSK with matched KeyIdx(%d), Dropped !!!\n", __FUNCTION__, KeyIdx);
			return false;
		}
	} else {
		WAPI_TRACE(WAPI_RX, "%s: Unicast decryption !!!\n", __FUNCTION__);
		if (pWapiSta->wapiUsk.keyId == KeyIdx && pWapiSta->wapiUsk.bSet) {
			WAPI_TRACE(WAPI_RX, "%s: Use USK for Decryption!!!\n", __FUNCTION__);
			if (precv_hdr->bWapiCheckPNInDecrypt) {
				if (GetFrameType(pskb->data) == WIFI_QOS_DATA_TYPE) {
					WapiGetLastRxUnicastPNForQoSData(TID, pWapiSta, lastRxPNforQoS);
					pLastRxPN = lastRxPNforQoS;
				} else
					pLastRxPN = pWapiSta->lastRxUnicastPN;
				if (!WapiComparePN(pRecvPN, pLastRxPN))
					return false;
				if (bQosData)
					WapiSetLastRxUnicastPNForQoSData(TID, pRecvPN, pWapiSta);
				else
					memcpy(pWapiSta->lastRxUnicastPN, pRecvPN, 16);
			} else
				memcpy(precv_hdr->WapiTempPN, pRecvPN, 16);

			if (check_fwstate(&padapter->mlmepriv, WIFI_STATION_STATE)) {
				if ((pRecvPN[0] & 0x1) == 0) {
					WAPI_TRACE(WAPI_ERR, "%s: Rx USK PN is not odd when Infra STA mode, Dropped !!!\n", __FUNCTION__);
					return false;
				}
			}

			pMicKey = pWapiSta->wapiUsk.micKey;
			pDataKey = pWapiSta->wapiUsk.dataKey;
		} else if (pWapiSta->wapiUskUpdate.keyId == KeyIdx && pWapiSta->wapiUskUpdate.bSet) {
			WAPI_TRACE(WAPI_RX, "%s: Use Updated USK for Decryption!!!\n", __FUNCTION__);
			if (pWapiSta->bAuthenticatorInUpdata)
				bUseUpdatedKey = true;
			else
				bUseUpdatedKey = false;

			if (bQosData)
				WapiSetLastRxUnicastPNForQoSData(TID, pRecvPN, pWapiSta);
			else
				memcpy(pWapiSta->lastRxUnicastPN, pRecvPN, 16);
			pMicKey = pWapiSta->wapiUskUpdate.micKey;
			pDataKey = pWapiSta->wapiUskUpdate.dataKey;
		} else {
			WAPI_TRACE(WAPI_ERR, "%s: No valid USK!!!KeyIdx=%d pWapiSta->wapiUsk.keyId=%d pWapiSta->wapiUskUpdate.keyId=%d\n", __FUNCTION__, KeyIdx, pWapiSta->wapiUsk.keyId,
				   pWapiSta->wapiUskUpdate.keyId);
			/* dump_buf(pskb->data,pskb->len); */
			return false;
		}
	}

	WAPI_DATA(WAPI_RX, "Decryption - DataKey", pDataKey, 16);
	WAPI_DATA(WAPI_RX, "Decryption - IV", pRecvPN, 16);
	WapiSMS4Decryption(pDataKey, pRecvPN, pSecData, DataLen, pSecData, &OutputLength);

	if (OutputLength != DataLen)
		WAPI_TRACE(WAPI_ERR, "%s:  Output Length Error!!!!\n", __FUNCTION__);

	WAPI_DATA(WAPI_RX, "Decryption - After decryption", pskb->data, pskb->len);

	DataLen -= padapter->wapiInfo.extra_postfix_len;

	SecCalculateMicSMS4(KeyIdx, pMicKey, pskb->data, pSecData, DataLen, MicBuffer);

	WAPI_DATA(WAPI_RX, "Decryption - MIC received", pRecvMic, SMS4_MIC_LEN);
	WAPI_DATA(WAPI_RX, "Decryption - MIC calculated", MicBuffer, SMS4_MIC_LEN);

	if (0 == memcmp(MicBuffer, pRecvMic, padapter->wapiInfo.extra_postfix_len)) {
		WAPI_TRACE(WAPI_RX, "%s: Check MIC OK!!\n", __FUNCTION__);
		if (bUseUpdatedKey) {
			/* delete the old key */
			if (IS_MCAST(pRA)) {
				WAPI_TRACE(WAPI_API, "%s(): AE use new update MSK!!\n", __FUNCTION__);
				pWapiSta->wapiMsk.keyId = pWapiSta->wapiMskUpdate.keyId;
				memcpy(pWapiSta->wapiMsk.dataKey, pWapiSta->wapiMskUpdate.dataKey, 16);
				memcpy(pWapiSta->wapiMsk.micKey, pWapiSta->wapiMskUpdate.micKey, 16);
				pWapiSta->wapiMskUpdate.bTxEnable = pWapiSta->wapiMskUpdate.bSet = false;
			} else {
				WAPI_TRACE(WAPI_API, "%s(): AE use new update USK!!\n", __FUNCTION__);
				pWapiSta->wapiUsk.keyId = pWapiSta->wapiUskUpdate.keyId;
				memcpy(pWapiSta->wapiUsk.dataKey, pWapiSta->wapiUskUpdate.dataKey, 16);
				memcpy(pWapiSta->wapiUsk.micKey, pWapiSta->wapiUskUpdate.micKey, 16);
				pWapiSta->wapiUskUpdate.bTxEnable = pWapiSta->wapiUskUpdate.bSet = false;
			}
		}
	} else {
		WAPI_TRACE(WAPI_ERR, "%s:  Check MIC Error, Dropped !!!!\n", __FUNCTION__);
		return false;
	}

	pos = pskb->data;
	memmove(pos + padapter->wapiInfo.extra_prefix_len, pos, IVOffset);
	skb_pull(pskb, padapter->wapiInfo.extra_prefix_len);

	WAPI_TRACE(WAPI_RX, "<=========%s\n", __FUNCTION__);

	return true;
}

u32	rtw_sms4_encrypt(_adapter *padapter, u8 *pxmitframe)
{

	u8	*pframe;
	u32 res = _SUCCESS;

	WAPI_TRACE(WAPI_TX, "=========>%s\n", __FUNCTION__);

	if ((!padapter->WapiSupport) || (!padapter->wapiInfo.bWapiEnable)) {
		WAPI_TRACE(WAPI_TX, "<========== %s, WAPI not supported or enabled!\n", __FUNCTION__);
		return _FAIL;
	}

	if (((struct xmit_frame *)pxmitframe)->buf_addr == NULL)
		return _FAIL;

	pframe = ((struct xmit_frame *)pxmitframe)->buf_addr + TXDESC_OFFSET;

	SecSWSMS4Encryption(padapter, pxmitframe);

	WAPI_TRACE(WAPI_TX, "<=========%s\n", __FUNCTION__);
	return res;
}

u32	rtw_sms4_decrypt(_adapter *padapter, u8 *precvframe)
{
	u8	*pframe;
	u32 res = _SUCCESS;

	WAPI_TRACE(WAPI_RX, "=========>%s\n", __FUNCTION__);

	if ((!padapter->WapiSupport) || (!padapter->wapiInfo.bWapiEnable)) {
		WAPI_TRACE(WAPI_RX, "<========== %s, WAPI not supported or enabled!\n", __FUNCTION__);
		return _FAIL;
	}


	/* drop packet when hw decrypt fail
	* return tempraily */
	return _FAIL;

	/* pframe=(unsigned char *)((union recv_frame*)precvframe)->u.hdr.rx_data; */

	if (false == SecSWSMS4Decryption(padapter, precvframe, &padapter->recvpriv)) {
		WAPI_TRACE(WAPI_ERR, "%s():SMS4 decrypt frame error\n", __FUNCTION__);
		return _FAIL;
	}

	WAPI_TRACE(WAPI_RX, "<=========%s\n", __FUNCTION__);
	return res;
}

#else

u32	rtw_sms4_encrypt(_adapter *padapter, u8 *pxmitframe)
{
	WAPI_TRACE(WAPI_TX, "=========>Dummy %s\n", __FUNCTION__);
	WAPI_TRACE(WAPI_TX, "<=========Dummy %s\n", __FUNCTION__);
	return _SUCCESS;
}

u32	rtw_sms4_decrypt(_adapter *padapter, u8 *precvframe)
{
	WAPI_TRACE(WAPI_RX, "=========>Dummy %s\n", __FUNCTION__);
	WAPI_TRACE(WAPI_RX, "<=========Dummy %s\n", __FUNCTION__);
	return _SUCCESS;
}

#endif

#endif
