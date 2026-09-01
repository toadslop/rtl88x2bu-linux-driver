// SPDX-License-Identifier: GPL-2.0
#include "host_mlme_80211d_types.h"

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint i = 0;
	const u8 *p = pbuf;

	if (limit < 1)
		return NULL;
	*len = 0;
	while (1) {
		sint tmp;

		if (*p == (u8)index) {
			*len = *(p + 1);
			return (u8 *)p;
		}
		tmp = *(p + 1);
		p += tmp + 2;
		i += tmp + 2;
		if (i >= limit)
			break;
	}
	return NULL;
}
