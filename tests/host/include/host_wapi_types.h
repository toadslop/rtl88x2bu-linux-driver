/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_WAPI_TYPES_H
#define HOST_WAPI_TYPES_H

#include <stddef.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define _TRUE 1
#define _FALSE 0
#define MAX_WAPI_IE_LEN 256

typedef struct _RT_WAPI_T {
	u8 wapiIE[MAX_WAPI_IE_LEN];
	u8 wapiIELength;
	u8 bWapiPSK;
} RT_WAPI_T;

typedef struct _adapter {
	RT_WAPI_T wapiInfo;
} _adapter;

u32 WapiComparePN(u8 *PN1, u8 *PN2);
void WapiSetIE(_adapter *padapter);
void host_wapi_adapter_init(_adapter *a);

#endif /* HOST_WAPI_TYPES_H */
