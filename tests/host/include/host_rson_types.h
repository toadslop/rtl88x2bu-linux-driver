/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_RSON_TYPES_H
#define HOST_RSON_TYPES_H

#include "host_types.h"
#include <string.h>

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6

#define RTW_RSON_SCORE_NOTCNNT 0x1
#define RTW_RSON_SCORE_MAX 0xFF
#define RTW_RSON_HC_NOTREADY 0xFF
#define RTW_RSON_DENYCONNECT 0x0

typedef long NDIS_802_11_RSSI;

struct rtw_rson_struct {
	u8 ver;
	u32 id;
	u8 hopcnt;
	u8 connectible;
	u8 loading;
	u8 res[16];
} __attribute__((__packed__));

static inline int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : _FALSE;
}

u8 rtw_cal_rson_score(struct rtw_rson_struct *cand_rson_data, NDIS_802_11_RSSI rssi);
int is_match_bssid(u8 *mac, u8 bssid_array[][6], int num);

#endif /* HOST_RSON_TYPES_H */
