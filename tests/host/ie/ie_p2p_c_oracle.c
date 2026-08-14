// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: P2P IE lookup helpers from core/rtw_ieee80211.c (W3-43). */
#include "host_ieee80211_types.h"

typedef unsigned int uint;

u8 *rtw_get_p2p_ie(const u8 *in_ie, int in_len, u8 *p2p_ie, uint *p2p_ielen)
{
	uint cnt;
	const u8 *p2p_ie_ptr = NULL;
	u8 eid, p2p_oui[4] = {0x50, 0x6F, 0x9A, 0x09};

	if (p2p_ielen)
		*p2p_ielen = 0;

	if (!in_ie || in_len < 0) {
		rtw_warn_on(1);
		return (u8 *)p2p_ie_ptr;
	}

	if (in_len <= 0)
		return (u8 *)p2p_ie_ptr;

	cnt = 0;

	while (cnt + 1 + 4 < (uint)in_len) {
		eid = in_ie[cnt];

		if (cnt + 1 + 4 >= MAX_IE_SZ) {
			rtw_warn_on(1);
			return NULL;
		}

		if (eid == WLAN_EID_VENDOR_SPECIFIC &&
		    _rtw_memcmp(&in_ie[cnt + 2], p2p_oui, 4) == _TRUE) {
			p2p_ie_ptr = in_ie + cnt;

			if (p2p_ie)
				_rtw_memcpy(p2p_ie, &in_ie[cnt], in_ie[cnt + 1] + 2);

			if (p2p_ielen)
				*p2p_ielen = in_ie[cnt + 1] + 2;

			break;
		} else {
			cnt += in_ie[cnt + 1] + 2;
		}
	}

	return (u8 *)p2p_ie_ptr;
}

u8 *rtw_get_p2p_attr(u8 *p2p_ie, uint p2p_ielen, u8 target_attr_id, u8 *buf_attr,
		     u32 *len_attr)
{
	u8 *attr_ptr = NULL;
	u8 *target_attr_ptr = NULL;
	u8 p2p_oui[4] = {0x50, 0x6F, 0x9A, 0x09};

	if (len_attr)
		*len_attr = 0;

	if (!p2p_ie || p2p_ielen <= 6 ||
	    (p2p_ie[0] != WLAN_EID_VENDOR_SPECIFIC) ||
	    (_rtw_memcmp(p2p_ie + 2, p2p_oui, 4) != _TRUE))
		return attr_ptr;

	attr_ptr = p2p_ie + 6;

	while ((attr_ptr - p2p_ie + 3) <= p2p_ielen) {
		u8 attr_id = *attr_ptr;
		u16 attr_data_len = RTW_GET_LE16(attr_ptr + 1);
		u16 attr_len = attr_data_len + 3;

		if ((attr_ptr - p2p_ie + attr_len) > p2p_ielen)
			break;

		if (attr_id == target_attr_id) {
			target_attr_ptr = attr_ptr;

			if (buf_attr)
				_rtw_memcpy(buf_attr, attr_ptr, attr_len);

			if (len_attr)
				*len_attr = attr_len;

			break;
		} else {
			attr_ptr += attr_len;
		}
	}

	return target_attr_ptr;
}
