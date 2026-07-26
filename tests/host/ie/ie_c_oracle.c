// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: core/rtw_ieee80211.c IE helpers */
#include "host_types.h"

typedef int sint;
typedef unsigned int uint;

#define _TRUE 1
#define _FAIL 0
#define _SUCCESS 1

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : 0;
}

void *_rtw_memmove(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
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

u8 *rtw_get_ie_ex(const u8 *in_ie, uint in_len, u8 eid, const u8 *oui, u8 oui_len,
		  u8 *ie, uint *ielen)
{
	uint cnt;
	const u8 *target_ie = NULL;

	if (ielen)
		*ielen = 0;

	if (!in_ie || in_len <= 0)
		return (u8 *)target_ie;

	cnt = 0;
	while (cnt < in_len) {
		if (eid == in_ie[cnt]
		    && (!oui || _rtw_memcmp(&in_ie[cnt + 2], oui, oui_len) == _TRUE)) {
			target_ie = &in_ie[cnt];
			if (ie)
				_rtw_memcpy(ie, &in_ie[cnt], in_ie[cnt + 1] + 2);
			if (ielen)
				*ielen = in_ie[cnt + 1] + 2;
			break;
		}
		cnt += in_ie[cnt + 1] + 2;
	}

	return (u8 *)target_ie;
}

int rtw_ies_remove_ie(u8 *ies, uint *ies_len, uint offset, u8 eid, u8 *oui, u8 oui_len)
{
	int ret = _FAIL;
	u8 *target_ie;
	u32 target_ielen;
	u8 *start;
	uint search_len;

	if (!ies || !ies_len || *ies_len <= offset)
		goto exit;

	start = ies + offset;
	search_len = *ies_len - offset;

	while (1) {
		target_ie = rtw_get_ie_ex(start, search_len, eid, oui, oui_len, NULL,
					    &target_ielen);
		if (target_ie && target_ielen) {
			u8 *remain_ies = target_ie + target_ielen;
			uint remain_len = search_len - (remain_ies - start);

			_rtw_memmove(target_ie, remain_ies, remain_len);
			*ies_len = *ies_len - target_ielen;
			ret = _SUCCESS;
			start = target_ie;
			search_len = remain_len;
		} else {
			break;
		}
	}
exit:
	return ret;
}
