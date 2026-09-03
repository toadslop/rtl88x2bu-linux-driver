// SPDX-License-Identifier: GPL-2.0
/*
 * Host L2 shims for W3-68 mgnt frame attrib tests.
 */
#include "host_mlme_ext_mgnt_attrib_types.h"

_adapter *host_mgnt_attrib_adapter;

int rtw_warn_on(int cond)
{
	(void)cond;
	return 0;
}

u8 rtw_get_mgntframe_raid(_adapter *adapter, unsigned char network_type)
{
	(void)adapter;
	return (network_type & WIRELESS_11B) ? RATEID_IDX_B : RATEID_IDX_G;
}

struct sta_info *rtw_get_stainfo(struct sta_priv *pstapriv, const u8 *hwaddr)
{
	(void)hwaddr;
	return pstapriv->fixture_sta;
}

int rtw_action_frame_parse(const u8 *frame, u32 frame_len, u8 *category,
			   u8 *action)
{
	const u8 *body;
	u16 fc;

	(void)frame_len;

	fc = le16_to_cpu(*(const u16 *)frame);
	if ((fc & 0x00fc) != 0x00d0) {
		if (category)
			*category = 0;
		if (action)
			*action = 0;
		return _FALSE;
	}

	body = frame + 24;
	if (category)
		*category = body[0];
	if (action)
		*action = body[1];
	return _TRUE;
}

void update_attrib_txbf_info(_adapter *padapter, struct pkt_attrib *pattrib,
			     struct sta_info *psta)
{
	(void)padapter;
	if (psta) {
		pattrib->txbf_g_id = psta->cmn.bf_info.g_id;
		pattrib->txbf_p_aid = psta->cmn.bf_info.p_aid;
	}
}

#ifdef CONFIG_CONCURRENT_MODE
u8 rtw_mi_buddy_check_fwstate(_adapter *padapter, int state)
{
	(void)state;
	return padapter->host_fixture.buddy_asoc;
}
#endif
