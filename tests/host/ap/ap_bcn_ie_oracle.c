// SPDX-License-Identifier: GPL-2.0
#include "host_ap_bcn_ie_types.h"
#include <stdlib.h>
#include <string.h>

#define WLAN_EID_TIM 5

static inline int bcn_bmp_is_set(const u8 *bmp, u8 bmp_len, u8 id)
{
	return (id / 8 < bmp_len) && (bmp[id / 8] & BIT(id % 8));
}

static inline int bcn_bmp_not_empty(const u8 *bmp, u8 bmp_len)
{
	u8 i;

	for (i = 0; i < bmp_len; i++)
		if (bmp[i])
			return 1;
	return 0;
}

void *rtw_malloc(size_t sz) { return malloc(sz); }
void rtw_mfree(void *p, size_t sz) { (void)sz; free(p); }

u8 *rtw_get_ie(const u8 *pbuf, sint index, sint *len, sint limit)
{
	sint tmp, i;
	const u8 *p;

	if (limit < 1)
		return NULL;
	p = pbuf;
	for (i = 0, *len = 0;;) {
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

/* Oracle copy of rtw_set_tim_ie from core/rtw_ap_rest.c — keep in sync to prevent drift. */
u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period, const u8 *tim_bmp, u8 tim_bmp_len,
		  u8 *tim_ie)
{
	u8 *p = tim_ie;
	u8 i, n1, n2, bmp_len;

	if (bcn_bmp_not_empty(tim_bmp, tim_bmp_len)) {
		for (i = 0; i < tim_bmp_len; i++)
			if (tim_bmp[i])
				break;
		n1 = i & 0xFE;
		for (i = tim_bmp_len - 1; i > 0; i--)
			if (tim_bmp[i])
				break;
		n2 = i;
		bmp_len = n2 - n1 + 1;
	} else {
		n1 = n2 = 0;
		bmp_len = 1;
	}

	*p++ = WLAN_EID_TIM;
	*p++ = 2 + 1 + bmp_len;
	*p++ = dtim_cnt;
	*p++ = dtim_period;
	*p++ = (bcn_bmp_is_set(tim_bmp, tim_bmp_len, 0) ? BIT0 : 0) | n1;
	memcpy(p, tim_bmp + n1, bmp_len);

	return 2 + 2 + 1 + bmp_len;
}
