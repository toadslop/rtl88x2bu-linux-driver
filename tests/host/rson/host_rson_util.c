// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"

u8 key_2char2num(u8 hch, u8 lch)
{
	hch = (hch >= 'a') ? (hch - 'a' + 10) : (hch - '0');
	lch = (lch >= 'a') ? (lch - 'a' + 10) : (lch - '0');
	return (hch << 4) | lch;
}

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp, i;
	const u8 *p;

	if (limit < 1)
		return NULL;

	p = pbuf;
	i = 0;
	*len = 0;
	while (1) {
		if (*p == index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += (tmp + 2);
		i += (tmp + 2);
		if (i >= limit)
			break;
	}
	return NULL;
}
