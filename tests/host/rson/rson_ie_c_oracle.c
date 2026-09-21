// SPDX-License-Identifier: GPL-2.0
#include "host_rson_types.h"
#include <string.h>

extern unsigned char RTW_RSON_OUI[];

static u8 rtw_rson_root_bssid_idx;
u8 rtw_rson_root_bssid[10][6];

void host_rson_set_root_bssid_count(u8 n)
{
	rtw_rson_root_bssid_idx = n;
}

void host_rson_set_root_bssid(u8 idx, const u8 mac[ETH_ALEN])
{
	if (idx < 10)
		memcpy(rtw_rson_root_bssid[idx], mac, ETH_ALEN);
}

int rtw_get_rson_struct(WLAN_BSSID_EX *bssid, struct rtw_rson_struct *rson_data)
{
	sint limit, len;
	u8 *p;

	if (!rson_data || !bssid)
		return -22;

	memset(rson_data, 0, sizeof(*rson_data));

	if (is_match_bssid(bssid->MacAddress, rtw_rson_root_bssid, rtw_rson_root_bssid_idx) ==
	    _TRUE) {
		rson_data->id = CONFIG_RTW_REPEATER_SON_ID;
		rson_data->ver = RTW_RSON_VER;
		rson_data->hopcnt = RTW_RSON_HC_ROOT;
		rson_data->connectible = RTW_RSON_ALLOWCONNECT;
		return _TRUE;
	}

	limit = (sint)bssid->IELength - _BEACON_IE_OFFSET_;
	for (p = bssid->IEs + _BEACON_IE_OFFSET_; ; p += (len + 2)) {
		p = rtw_get_ie(p, _VENDOR_SPECIFIC_IE_, &len, limit);
		limit -= len;
		if (!p || !len)
			break;
		if (_rtw_memcmp(p + 2, RTW_RSON_OUI, 3) == _TRUE && rtw_rson_varify_ie(p)) {
			p = p + 2 + 3;
			rson_data->ver = *p++;
			rson_data->id = le32_to_cpup((__le32 *)p);
			p += 4;
			rson_data->hopcnt = *p++;
			rson_data->connectible = *p++;
			rson_data->loading = *p;
			return _TRUE;
		}
	}
	return -74;
}

static u8 rtw_rson_block_bssid_idx;
u8 rtw_rson_block_bssid[10][6];

void host_rson_set_block_bssid_count(u8 n)
{
	rtw_rson_block_bssid_idx = n;
}

void host_rson_set_block_bssid(u8 idx, const u8 mac[ETH_ALEN])
{
	if (idx < 10)
		memcpy(rtw_rson_block_bssid[idx], mac, ETH_ALEN);
}

int rtw_rson_choose(struct wlan_network **candidate, struct wlan_network *competitor)
{
	s16 comp_score, cand_score;
	struct rtw_rson_struct rson_cand, rson_comp;

	if (is_match_bssid(competitor->network.MacAddress, rtw_rson_block_bssid,
			   rtw_rson_block_bssid_idx) == _TRUE)
		return _FALSE;

	if (!competitor ||
	    rtw_get_rson_struct(&(competitor->network), &rson_comp) != _TRUE ||
	    rson_comp.id != CONFIG_RTW_REPEATER_SON_ID)
		return _FALSE;

	comp_score = rtw_cal_rson_score(&rson_comp, competitor->network.Rssi);
	if (comp_score == RTW_RSON_SCORE_NOTCNNT)
		return _FALSE;

	if (!*candidate)
		return _TRUE;
	if (rtw_get_rson_struct(&((*candidate)->network), &rson_cand) != _TRUE)
		return _FALSE;

	cand_score = rtw_cal_rson_score(&rson_cand, (*candidate)->network.Rssi);
	if (comp_score - cand_score > 8)
		return _TRUE;
	return _FALSE;
}

u32 rtw_rson_append_ie(_adapter *padapter, unsigned char *pframe, u32 *len)
{
	u8 *ptr, ie_len;
	struct dvobj_priv *pdvobj = &padapter->dvobj;

	if (!pframe)
		return 0;
	ptr = pframe;
	*ptr++ = _VENDOR_SPECIFIC_IE_;
	*ptr++ = ie_len = 3 + (u8)sizeof(pdvobj->rson_data);
	memcpy(ptr, RTW_RSON_OUI, 3);
	ptr += 3;
	*ptr++ = pdvobj->rson_data.ver;
	*(u32 *)ptr = cpu_to_le32(pdvobj->rson_data.id);
	ptr += 4;
	*ptr++ = pdvobj->rson_data.hopcnt;
	*ptr++ = pdvobj->rson_data.connectible;
	*ptr++ = pdvobj->rson_data.loading;
	memcpy(ptr, pdvobj->rson_data.res, sizeof(pdvobj->rson_data.res));
	*len += ie_len + 2;
	return ie_len;
}
