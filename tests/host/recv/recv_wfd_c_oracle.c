// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: WFD IE helpers from core/rtw_ieee80211.c (W3-39 PR3). */
#include "host_recv_types.h"

typedef unsigned int uint;

static void rtw_warn_on(int condition)
{
	(void)condition;
}

static int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : 0;
}

static void *_rtw_memmove(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
}

u8 *rtw_get_wfd_ie(const u8 *in_ie, int in_len, u8 *wfd_ie, uint *wfd_ielen)
{
	uint cnt;
	const u8 *wfd_ie_ptr = NULL;
	u8 eid, wfd_oui[4] = {0x50, 0x6F, 0x9A, 0x0A};

	if (wfd_ielen)
		*wfd_ielen = 0;

	if (!in_ie || in_len < 0) {
		rtw_warn_on(1);
		return (u8 *)wfd_ie_ptr;
	}

	if (in_len <= 0)
		return (u8 *)wfd_ie_ptr;

	cnt = 0;

	while (cnt + 1 + 4 < (uint)in_len) {
		eid = in_ie[cnt];

		if (cnt + 1 + 4 >= MAX_IE_SZ) {
			rtw_warn_on(1);
			return NULL;
		}

		if (eid == WLAN_EID_VENDOR_SPECIFIC &&
		    _rtw_memcmp(&in_ie[cnt + 2], wfd_oui, 4) == _TRUE) {
			wfd_ie_ptr = in_ie + cnt;

			if (wfd_ie)
				_rtw_memcpy(wfd_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (wfd_ielen)
				*wfd_ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}
	}

	return (u8 *)wfd_ie_ptr;
}

uint rtw_del_wfd_ie(u8 *ies, uint ies_len_ori, const char *msg)
{
	u8 *target_ie;
	u32 target_ie_len;
	uint ies_len = ies_len_ori;

	(void)msg;

	while (1) {
		target_ie = rtw_get_wfd_ie(ies, (int)ies_len, NULL, &target_ie_len);
		if (target_ie && target_ie_len) {
			u8 *next_ie = target_ie + target_ie_len;
			uint remain_len = ies_len - (uint)(next_ie - ies);

			_rtw_memmove(target_ie, next_ie, remain_len);
			_rtw_memset(target_ie + remain_len, 0, target_ie_len);
			ies_len -= target_ie_len;
		} else {
			break;
		}
	}

	return ies_len;
}
