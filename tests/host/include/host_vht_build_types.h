/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_VHT_BUILD_TYPES_H
#define HOST_VHT_BUILD_TYPES_H

#include "host_types.h"

#include <stdbool.h>

typedef unsigned int uint;

#define EID_VHTOperation 192
#define CHANNEL_WIDTH_20 0
#define CHANNEL_WIDTH_80 2
#define BW_CAP_80M (1U << 4)
#define BW_CAP_160M (1U << 5)
#define HAL_PRIME_CHNL_OFFSET_LOWER 1

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

#define BW_MODE_5G(bw_mode) ((bw_mode) >> 4)
#define REGSTY_BW_5G(r) BW_MODE_5G((r)->bw_mode)

struct registry_priv {
	u8 bw_mode;
};

struct vht_priv {
	u8 vht_mcs_map[2];
};

struct mlme_priv {
	struct vht_priv vhtpriv;
};

typedef struct {
	struct registry_priv registrypriv;
	struct mlme_priv mlmepriv;
} _adapter;

extern _adapter host_vht_build_adapter;
extern u8 host_vht_build_hal_bw_cap;

u8 *rtw_set_ie(u8 *pbuf, int index, uint len, const u8 *source, uint *frlen);
bool hal_chk_bw_cap(_adapter *adapter, u8 cap);
u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset);

u32 rtw_build_vht_operation_ie(_adapter *padapter, u8 *pbuf, u8 channel);

#endif /* HOST_VHT_BUILD_TYPES_H */
