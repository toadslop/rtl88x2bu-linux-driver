// SPDX-License-Identifier: GPL-2.0
#include "host_vht_rest.h"

_adapter *host_vht_rest_adapter;

u8 *rtw_get_ie(const u8 *pbuf, sint index, uint *len, sint limit)
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

u8 *rtw_set_ie(u8 *pbuf, sint index, uint len, const u8 *source, uint *frlen)
{
	*pbuf = (u8)index;
	*(pbuf + 1) = (u8)len;
	if (len)
		_rtw_memcpy(pbuf + 2, source, len);
	if (frlen)
		*frlen += len + 2;
	return pbuf + len + 2;
}

void rtw_vht_use_default_setting(_adapter *padapter) { (void)padapter; }

u32 rtw_build_vht_cap_ie(_adapter *padapter, u8 *pbuf)
{
	uint len = 0;

	return (u32)(rtw_set_ie(pbuf, EID_VHTCapability, VHT_CAP_IE_LEN,
				 padapter->host_fixture.cap_ie, &len) - pbuf);
}

u32 rtw_build_vht_op_mode_notify_ie(_adapter *padapter, u8 *pbuf, u8 bw)
{
	uint len = 0;
	u8 opmode = padapter->host_fixture.opmode_notify;

	(void)bw;
	return (u32)(rtw_set_ie(pbuf, EID_OpModeNotification, 1, &opmode, &len) - pbuf);
}

u8 hal_largest_bw(_adapter *padapter, u8 bw_cap)
{
	(void)bw_cap;
	return padapter->host_fixture.hal_max_bw;
}

bool rtw_chset_is_chbw_valid(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset,
			     u8 a, u8 b)
{
	(void)ch_set;
	(void)ch;
	(void)offset;
	(void)a;
	(void)b;
	return bw <= host_vht_rest_adapter->host_fixture.chset_max_bw;
}

bool rtw_chset_is_chbw_non_ocp(RT_CHANNEL_INFO *ch_set, u8 ch, u8 bw, u8 offset)
{
	(void)ch_set;
	(void)ch;
	(void)bw;
	(void)offset;
	return false;
}

u8 rtw_rfctl_dfs_domain_unknown(struct rf_ctl_t *rfctl)
{
	(void)rfctl;
	return _TRUE;
}

u8 rtw_get_center_ch(u8 ch, u8 bw, u8 offset)
{
	if (bw == CHANNEL_WIDTH_80 && ch >= 36 && ch <= 48 && ch % 4 == 0)
		return 42;
	if (bw == CHANNEL_WIDTH_40 && offset == HAL_PRIME_CHNL_OFFSET_LOWER)
		return ch + 2;
	return ch;
}
