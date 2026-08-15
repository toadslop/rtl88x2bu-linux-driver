/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_VHT_MCS_TYPES_H
#define HOST_VHT_MCS_TYPES_H

#include "host_autoconf.h"
#include <string.h>

typedef unsigned char u8;
typedef int sint;
typedef unsigned int uint;

#define _TRUE 1
#define _FALSE 0
#define RTW_INFO(...) do { } while (0)
#define FUNC_ADPT_FMT "%p"
#define FUNC_ADPT_ARG(a) (void *)(a)

#define WLAN_EID_VHT_OPERATION 192
#define EID_VHTOperation WLAN_EID_VHT_OPERATION
#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_80 2

#define LE_BITS_TO_1BYTE(p, o, l) (((*((u8 *)(p)) >> (o)) & ((1U << (l)) - 1)))
#define SET_BITS_TO_LE_1BYTE(p, o, l, v) do { \
	u8 *__x = (u8 *)(p); \
	u8 __m = (((1U << (l)) - 1) << (o)); \
	*__x = (*__x & ~__m) | (((v) & ((1U << (l)) - 1)) << (o)); \
} while (0)

#define GET_VHT_OPERATION_ELE_CHL_WIDTH(p) LE_BITS_TO_1BYTE(p, 0, 8)
#define SET_VHT_OPERATION_ELE_CHL_WIDTH(p, v) SET_BITS_TO_LE_1BYTE(p, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(p, v) SET_BITS_TO_LE_1BYTE((p) + 1, 0, 8, v)
#define SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(p, v) SET_BITS_TO_LE_1BYTE((p) + 2, 0, 8, v)

struct _adapter { u8 _pad; };
typedef struct _adapter _adapter;

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit);
void rtw_ies_get_chbw(u8 *ies, int ies_len, u8 *ch, u8 *bw, u8 *offset, u8 ht,
		      u8 vht);
void rtw_check_for_vht20(_adapter *adapter, u8 *ies, int ies_len);

#endif /* HOST_VHT_MCS_TYPES_H */
