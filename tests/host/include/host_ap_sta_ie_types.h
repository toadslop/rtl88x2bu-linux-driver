/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_AP_STA_IE_TYPES_H
#define HOST_AP_STA_IE_TYPES_H

#include "host_types.h"

#define WLAN_CAPABILITY_SHORT_PREAMBLE (1 << 5)
#define WLAN_STA_SHORT_PREAMBLE (1U << 7)

#define RTW_GET_LE16(a) ((u16)(((a)[1] << 8) | (a)[0]))

struct cmn_sta_info {
	unsigned char mac_addr[6];
};

struct sta_info {
	struct cmn_sta_info cmn;
	unsigned short capability;
	int flags;
};

struct _adapter {
	unsigned char pad;
};

typedef struct _adapter _adapter;

void rtw_ap_parse_sta_capability(_adapter *adapter, struct sta_info *sta,
				 unsigned char *cap);

#endif /* HOST_AP_STA_IE_TYPES_H */
