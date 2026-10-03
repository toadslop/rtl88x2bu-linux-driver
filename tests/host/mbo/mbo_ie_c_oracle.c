// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_mbo_types.h"

static u8 wfa_mbo_oui[] = {0x50, 0x6F, 0x9A, 0x16};

#define rtw_mbo_get_oui(p) ((u8 *)(p) + 2)
#define rtw_mbo_get_disallow_res(p) ((u8 *)(p) + 3)
#define rtw_mbo_set_1byte_ie(p, v, l) rtw_set_fixed_ie((p), 1, (v), (l))
#define rtw_mbo_set_4byte_ie(p, v, l) rtw_set_fixed_ie((p), 4, (v), (l))

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

u8 *rtw_set_fixed_ie(u8 *pbuf, unsigned int len, u8 *source, unsigned int *frlen)
{
	_rtw_memcpy(pbuf, source, len);
	*frlen += len;
	return pbuf + len;
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

u32 host_mbo_attr_sz_get(_adapter *padapter, u8 id)
{
	u32 len = 0;

	switch (id) {
	case RTW_MBO_ATTR_NPREF_CH_RPT_ID: {
		struct npref_ch_rtp *prpt = &adapter_to_rfctl(padapter)->ch_rtp;
		u32 i;

		for (i = 0; i < prpt->nm_of_rpt; i++) {
			struct npref_ch *pch = &prpt->ch_rpt[i];
			u32 attr_len = (u32)pch->nm_of_ch + 3;

			len += attr_len + 2;
		}
		break;
	}
	case RTW_MBO_ATTR_CELL_DATA_CAP_ID:
	case RTW_MBO_ATTR_TRANS_REJ_ID:
		len = 3;
		break;
	default:
		break;
	}
	return len;
}

void host_mbo_build_mbo_ie_hdr(u8 **pframe, struct pkt_attrib *pattrib,
			       u8 payload_len)
{
	u8 eid = RTW_MBO_EID;
	u8 len = payload_len + 4;

	*pframe = rtw_mbo_set_1byte_ie(*pframe, &eid, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_1byte_ie(*pframe, &len, &(pattrib->pktlen));
	*pframe = rtw_mbo_set_4byte_ie(*pframe, wfa_mbo_oui, &(pattrib->pktlen));
}

u8 host_mbo_disallowed_network(struct wlan_network *pnetwork)
{
	u8 *p;
	u32 attr_len = 0;

	if (!pnetwork)
		return _FALSE;
	p = host_mbo_attrs_get(pnetwork->network.IEs, pnetwork->network.IELength,
			       RTW_MBO_ATTR_ASSOC_DISABLED_ID, &attr_len);
	if (!p)
		return _FALSE;
	RTW_INFO("MBO : block " MAC_FMT " reason %d\n",
		 MAC_ARG(pnetwork->network.MacAddress),
		 *rtw_mbo_get_disallow_res(p));
	return _TRUE;
}

u8 host_mbo_non_pref_chan_exist(struct npref_ch *pch, u8 ch)
{
	u32 i;

	for (i = 0; i < pch->nm_of_ch; i++) {
		if (pch->chs[i] == ch)
			return _TRUE;
	}
	return _FALSE;
}

void host_mbo_adapter_clear(_adapter *a)
{
	_rtw_memset(a, 0, sizeof(*a));
}

void host_mbo_seed_npref(_adapter *a, u8 rpt_idx, u8 op_class, u8 ch_count,
			 const u8 *chs, u8 preference, u8 reason)
{
	struct npref_ch_rtp *prpt = &adapter_to_rfctl(a)->ch_rtp;
	struct npref_ch *pch;
	u8 i;

	if (rpt_idx >= RTW_MBO_MAX_CH_RPT_NUM)
		return;
	pch = &prpt->ch_rpt[rpt_idx];
	pch->op_class = op_class;
	pch->preference = preference;
	pch->reason = reason;
	pch->nm_of_ch = ch_count;
	for (i = 0; i < ch_count && i < RTW_MBO_MAX_CH_LIST_NUM; i++)
		pch->chs[i] = chs[i];
	if (prpt->nm_of_rpt <= rpt_idx)
		prpt->nm_of_rpt = rpt_idx + 1;
}
