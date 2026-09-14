// SPDX-License-Identifier: GPL-2.0
#include "host_vht_build_types.h"

#include <stdbool.h>

_adapter host_vht_build_adapter;
u8 host_vht_build_hal_bw_cap;

u8 *rtw_set_ie(u8 *pbuf, int index, uint len, const u8 *source, uint *frlen)
{
	*pbuf = (u8)index;
	*(pbuf + 1) = (u8)len;
	if (len)
		_rtw_memcpy(pbuf + 2, source, len);
	if (frlen)
		*frlen += len + 2;
	return pbuf + len + 2;
}

bool hal_chk_bw_cap(_adapter *adapter, u8 cap)
{
	(void)adapter;
	return (host_vht_build_hal_bw_cap & cap) == cap;
}

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset)
{
	if (bw == CHANNEL_WIDTH_80 && ch >= 36 && ch <= 48 && ch % 4 == 0)
		return 42;
	if (bw == CHANNEL_WIDTH_80 && ch == 149)
		return 155;
	if (offset == HAL_PRIME_CHNL_OFFSET_LOWER && bw != CHANNEL_WIDTH_20)
		return ch + 2;
	return ch;
}
