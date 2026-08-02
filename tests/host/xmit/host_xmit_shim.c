// SPDX-License-Identifier: GPL-2.0
/*
 * Host shims for rtw_xmit_rest L2 tests (W3-40).
 */

#include "host_xmit_types.h"

static inline bool rtw_macid_is_set(struct macid_bmp *map, u8 id)
{
	if (id < 32)
		return map->m0 & BIT(id);
	return false;
}

bool rtw_macid_is_used(struct macid_ctl_t *macid_ctl, u8 id)
{
	return rtw_macid_is_set(&macid_ctl->used, id);
}

bool rtw_macid_is_iface_shared(struct macid_ctl_t *macid_ctl, u8 id)
{
	int i;
	u8 iface_bmp = 0;

	for (i = 0; i < CONFIG_IFACE_NUMBER; i++) {
		if (rtw_macid_is_set(&macid_ctl->if_g[i], id)) {
			if (iface_bmp)
				return true;
			iface_bmp |= BIT(i);
		}
	}
	return false;
}

bool rtw_macid_is_iface_specific(struct macid_ctl_t *macid_ctl, u8 id,
				 _adapter *adapter)
{
	int i;
	u8 iface_bmp = 0;

	for (i = 0; i < CONFIG_IFACE_NUMBER; i++) {
		if (rtw_macid_is_set(&macid_ctl->if_g[i], id)) {
			if (iface_bmp || i != adapter->iface_id)
				return false;
			iface_bmp |= BIT(i);
		}
	}

	return iface_bmp ? true : false;
}

bool hal_is_bw_support(_adapter *adapter, u8 bw)
{
	return adapter->hal_bw_cap & ch_width_to_bw_cap(bw);
}

const u8 _ch_width_to_bw_cap[CHANNEL_WIDTH_MAX] = {
	[CHANNEL_WIDTH_20] = BW_CAP_20M,
	[CHANNEL_WIDTH_40] = BW_CAP_40M,
	[CHANNEL_WIDTH_80] = BW_CAP_80M,
	[CHANNEL_WIDTH_160] = BW_CAP_160M,
	[CHANNEL_WIDTH_80_80] = BW_CAP_80_80M,
	[CHANNEL_WIDTH_5] = BW_CAP_5M,
	[CHANNEL_WIDTH_10] = BW_CAP_10M,
};
