// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"

static u8 key_char2num(u8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else
		return 0xff;
}

u8 key_2char2num(u8 hch, u8 lch)
{
	return (key_char2num(hch) << 4) | key_char2num(lch);
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
