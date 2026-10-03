// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-109 PN/IE helpers (provenance: core/rtw_wapi.c). */
#include <string.h>
#include "host_wapi_types.h"

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : _FALSE;
}

void _rtw_memcpy(void *dst, const void *src, size_t n)
{
	memcpy(dst, src, n);
}

void _rtw_memset(void *p, int v, size_t n)
{
	memset(p, v, n);
}

void host_wapi_adapter_init(_adapter *a)
{
	_rtw_memset(a, 0, sizeof(*a));
	WapiResetAllCamEntry(a);
}

void WapiSetIE(_adapter *padapter)
{
	RT_WAPI_T *pWapiInfo = &padapter->wapiInfo;
	u16 protocolVer = 1;
	u16 akmCnt = 1;
	u16 suiteCnt = 1;
	u16 capability = 0;
	u8 OUI[3] = { 0x00, 0x14, 0x72 };

	pWapiInfo->wapiIELength = 0;
	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, &protocolVer, 2);
	pWapiInfo->wapiIELength += 2;
	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, &akmCnt, 2);
	pWapiInfo->wapiIELength += 2;

	if (pWapiInfo->bWapiPSK) {
		memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, OUI, 3);
		pWapiInfo->wapiIELength += 3;
		pWapiInfo->wapiIE[pWapiInfo->wapiIELength] = 0x2;
		pWapiInfo->wapiIELength += 1;
	} else {
		memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, OUI, 3);
		pWapiInfo->wapiIELength += 3;
		pWapiInfo->wapiIE[pWapiInfo->wapiIELength] = 0x1;
		pWapiInfo->wapiIELength += 1;
	}

	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, &suiteCnt, 2);
	pWapiInfo->wapiIELength += 2;
	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, OUI, 3);
	pWapiInfo->wapiIELength += 3;
	pWapiInfo->wapiIE[pWapiInfo->wapiIELength] = 0x1;
	pWapiInfo->wapiIELength += 1;

	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, OUI, 3);
	pWapiInfo->wapiIELength += 3;
	pWapiInfo->wapiIE[pWapiInfo->wapiIELength] = 0x1;
	pWapiInfo->wapiIELength += 1;

	memcpy(pWapiInfo->wapiIE + pWapiInfo->wapiIELength, &capability, 2);
	pWapiInfo->wapiIELength += 2;
}

u32 WapiComparePN(u8 *PN1, u8 *PN2)
{
	char i;

	if ((NULL == PN1) || (NULL == PN2))
		return 1;

	if ((PN2[15] - PN1[15]) & 0x80)
		return 1;

	for (i = 16; i > 0; i--) {
		if (PN1[i - 1] == PN2[i - 1])
			continue;
		else if (PN1[i - 1] > PN2[i - 1])
			return 1;
		else
			return 0;
	}

	return 0;
}

u8 WapiGetEntryForCamWrite(_adapter *padapter, u8 *pMacAddr, u8 KID, u8 IsMsk)
{
	RT_WAPI_T *pWapiInfo = &padapter->wapiInfo;
	u8 i;
	u8 ret = 0xff;

	for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++) {
		if (pWapiInfo->wapiCamEntry[i].IsUsed &&
		    (_rtw_memcmp(pMacAddr, pWapiInfo->wapiCamEntry[i].PeerMacAddr,
				 ETH_ALEN) == _TRUE) &&
		    pWapiInfo->wapiCamEntry[i].keyidx == KID &&
		    pWapiInfo->wapiCamEntry[i].type == IsMsk) {
			ret = pWapiInfo->wapiCamEntry[i].entry_idx;
			break;
		}
	}

	if (i == WAPI_CAM_ENTRY_NUM) {
		for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++) {
			if (pWapiInfo->wapiCamEntry[i].IsUsed == 0) {
				pWapiInfo->wapiCamEntry[i].IsUsed = 1;
				pWapiInfo->wapiCamEntry[i].type = IsMsk;
				pWapiInfo->wapiCamEntry[i].keyidx = KID;
				_rtw_memcpy(pWapiInfo->wapiCamEntry[i].PeerMacAddr,
					    pMacAddr, ETH_ALEN);
				ret = pWapiInfo->wapiCamEntry[i].entry_idx;
				break;
			}
		}
	}

	return ret;
}

u8 WapiGetEntryForCamClear(_adapter *padapter, u8 *pPeerMac, u8 keyid, u8 IsMsk)
{
	RT_WAPI_T *pWapiInfo = &padapter->wapiInfo;
	u8 i;

	for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++) {
		if (pWapiInfo->wapiCamEntry[i].IsUsed &&
		    (_rtw_memcmp(pPeerMac, pWapiInfo->wapiCamEntry[i].PeerMacAddr,
				 ETH_ALEN) == _TRUE) &&
		    pWapiInfo->wapiCamEntry[i].keyidx == keyid &&
		    pWapiInfo->wapiCamEntry[i].type == IsMsk) {
			pWapiInfo->wapiCamEntry[i].IsUsed = 0;
			pWapiInfo->wapiCamEntry[i].keyidx = 2;
			_rtw_memset(pWapiInfo->wapiCamEntry[i].PeerMacAddr, 0, ETH_ALEN);

			return pWapiInfo->wapiCamEntry[i].entry_idx;
		}
	}

	return 0xff;
}

void WapiResetAllCamEntry(_adapter *padapter)
{
	RT_WAPI_T *pWapiInfo = &padapter->wapiInfo;
	int i;

	for (i = 0; i < WAPI_CAM_ENTRY_NUM; i++) {
		_rtw_memset(pWapiInfo->wapiCamEntry[i].PeerMacAddr, 0, ETH_ALEN);
		pWapiInfo->wapiCamEntry[i].IsUsed = 0;
		pWapiInfo->wapiCamEntry[i].keyidx = 2;
		pWapiInfo->wapiCamEntry[i].entry_idx = (u8)(4 + i * 2);
	}
}
