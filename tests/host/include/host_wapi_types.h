/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_WAPI_TYPES_H
#define HOST_WAPI_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define WAPI_CAM_ENTRY_NUM 14
#define MAX_WAPI_IE_LEN 256

typedef struct _RT_WAPI_CAM_ENTRY {
	u8 IsUsed;
	u8 entry_idx;
	u8 keyidx;
	u8 PeerMacAddr[ETH_ALEN];
	u8 type;
} RT_WAPI_CAM_ENTRY;

typedef struct _RT_WAPI_T {
	u8 wapiIE[MAX_WAPI_IE_LEN];
	u8 wapiIELength;
	u8 bWapiPSK;
	RT_WAPI_CAM_ENTRY wapiCamEntry[WAPI_CAM_ENTRY_NUM];
} RT_WAPI_T;

typedef struct _adapter {
	RT_WAPI_T wapiInfo;
} _adapter;

int _rtw_memcmp(const void *s1, const void *s2, size_t n);

u32 WapiComparePN(u8 *PN1, u8 *PN2);
void WapiSetIE(_adapter *padapter);
u8 WapiGetEntryForCamWrite(_adapter *padapter, u8 *pMacAddr, u8 KID, u8 IsMsk);
u8 WapiGetEntryForCamClear(_adapter *padapter, u8 *pPeerMac, u8 keyid, u8 IsMsk);
void WapiResetAllCamEntry(_adapter *padapter);
void host_wapi_adapter_init(_adapter *a);

#endif /* HOST_WAPI_TYPES_H */
