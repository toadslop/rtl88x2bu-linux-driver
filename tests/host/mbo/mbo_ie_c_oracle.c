// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_mbo_types.h"

static u8 wfa_mbo_oui[] = {0x50, 0x6F, 0x9A, 0x16};

#define rtw_mbo_get_oui(p) ((u8 *)(p) + 2)

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : 0;
}

u8 *rtw_get_ie(const u8 *pbuf, s32 index, s32 *len, s32 limit)
{
	s32 tmp, i;
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

u8 *host_mbo_ie_get(u8 *pie, u32 *plen, u32 limit)
{
	const u8 *p = pie;
	u32 tmp, i;

	if (limit <= 1)
		return NULL;
	i = 0;
	*plen = 0;
	while (1) {
		if ((*p == _VENDOR_SPECIFIC_IE_) &&
		    (_rtw_memcmp(rtw_mbo_get_oui(p), wfa_mbo_oui, 4))) {
			*plen = *(p + 1);
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

u8 *host_mbo_attrs_get(u8 *pie, u32 limit, u8 attr_id, u32 *attr_len)
{
	u8 *p = NULL;
	u32 plen = 0;
	s32 alen = 0;

	if (!pie || limit <= 1)
		goto exit;
	if ((p = host_mbo_ie_get(pie, &plen, limit)) == NULL)
		goto exit;
	p = p + 2 + sizeof(wfa_mbo_oui);
	plen -= 4;
	if ((p = rtw_get_ie(p, attr_id, &alen, (s32)plen)) == NULL)
		goto exit;
	*attr_len = (u32)alen;
exit:
	return p;
}
