// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_wnm_types.h"

#define ETH_ALEN 6
#define RTW_WLAN_ACTION_WNM_NB_RPT_ELEM 0x34
#define WNM_BTM_CAND_PREF_SUBEID 0x03
#define RTW_MAX_NB_RPT_NUM 8

static u32 wnm_default_validity_time = 6000;
static u32 wnm_default_disassoc_time = 5000;

int _rtw_memcmp(const void *s1, const void *s2, size_t n)
{
	return memcmp(s1, s2, n) == 0 ? _TRUE : _FALSE;
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

#define wnm_btm_bss_term_inc(p) (*((u8 *)((p) + 3)) & BSS_TERMINATION_INCLUDED)
#define wnm_btm_ess_disassoc_im(p) (*((u8 *)((p) + 3)) & ESS_DISASSOC_IMMINENT)
#define wnm_btm_dialog_token(p) (*((u8 *)((p) + 2)))
#define wnm_btm_req_mode(p) (*((u8 *)((p) + 3)))
#define wnm_btm_disassoc_timer(p) (*((u16 *)((p) + 4)))
#define wnm_btm_valid_interval(p) (*((u8 *)((p) + 6)))
#define wnm_btm_term_duration_offset(p) ((p) + 7)

void host_wnm_btm_req_hdr_parsing(u8 *pframe, struct btm_req_hdr *phdr)
{
	u8 *pos;

	if (!pframe || !phdr)
		return;

	_rtw_memset(phdr, 0, sizeof(*phdr));
	phdr->dialog_token = wnm_btm_dialog_token(pframe);
	phdr->req_mode = wnm_btm_req_mode(pframe);
	phdr->disassoc_timer = wnm_btm_disassoc_timer(pframe);
	phdr->validity_interval = wnm_btm_valid_interval(pframe);
	if (wnm_btm_bss_term_inc(pframe)) {
		pos = wnm_btm_term_duration_offset(pframe);
		if (*pos == WNM_BTM_TERM_DUR_SUBEID) {
			phdr->term_duration.id = *pos;
			phdr->term_duration.len = *(pos + 1);
			memcpy(&phdr->term_duration.tsf, pos + 2, sizeof(u64));
			memcpy(&phdr->term_duration.duration, pos + 10, sizeof(u16));
		}
	}
}

u32 host_wnm_btm_candidates_offset_get(u8 *pframe)
{
	u32 offset = 0;

	if (!pframe)
		return 0;

	offset += 7;

	if (wnm_btm_bss_term_inc(pframe))
		offset += 12;

	if (wnm_btm_ess_disassoc_im(pframe))
		offset = 1 + *(pframe + offset);

	return offset;
}

static u32 host_passing_ms;

#define wnm_btm_rsp_status(p) (*((u8 *)((p) + 3)))

void host_wnm_set_passing_ms(u32 ms)
{
	host_passing_ms = ms;
}

s32 rtw_get_passing_time_ms(systime start)
{
	(void)start;
	return (s32)host_passing_ms;
}

u8 host_wnm_btm_candidate_validity(struct btm_rpt_cache *pcache, u8 flag)
{
	u8 is_validity = _TRUE;
	u32 req_validity_time = (u32)rtw_get_passing_time_ms(pcache->req_stime);

	if ((flag & (1 << 0)) && (req_validity_time > pcache->validity_time))
		is_validity = _FALSE;

	if ((flag & (1 << 1)) && (req_validity_time > pcache->disassoc_time))
		is_validity = _FALSE;

	return is_validity;
}

u32 host_wnm_btm_rsp_candidates_sz_get(_adapter *padapter, u8 *pframe,
				       u32 frame_len)
{
	u32 num = 0, sz = 0;
	u8 status;

	(void)padapter;

	if (!pframe || frame_len <= 5)
		goto exit;

	status = wnm_btm_rsp_status(pframe);
	if (((status != 0) && (status != 6)) || (frame_len < 23))
		goto exit;

	if (status == 0)
		num = (frame_len - 5 - ETH_ALEN) / 18;
	else
		num = (frame_len - 5) / 18;
	sz = sizeof(struct wnm_btm_cant) * num;
exit:
	return sz;
}

u8 host_wnm_nb_elem_parsing(u8 *pdata, u32 data_len, u8 from_btm,
			    u32 *nb_rpt_num, u8 *nb_rpt_is_same,
			    struct roam_nb_info *pnb,
			    struct wnm_btm_cant *pcandidates)
{
	u8 bfound = _FALSE;
	u8 *ptr, *pend, *op;
	u32 elem_len, subelem_len, op_len = 0;
	u32 i, nb_rpt_entries = 0;
	struct nb_rpt_hdr *pie;
	struct wnm_btm_cant *pcandidate;

	if (!pdata || !pnb)
		return 1;

	if (from_btm && !pcandidates)
		return 1;

	ptr = pdata;
	pend = ptr + data_len;
	elem_len = data_len;
	subelem_len = (u32)*(pdata + 1);

	for (i = 0; i < RTW_MAX_NB_RPT_NUM; i++) {
		if (((ptr + 7) > pend) || (elem_len < subelem_len))
			break;

		if (*ptr != RTW_WLAN_ACTION_WNM_NB_RPT_ELEM)
			break;

		pie = (struct nb_rpt_hdr *)ptr;
		op = NULL;
		if (from_btm)
			op = rtw_get_ie((u8 *)(ptr + 15), WNM_BTM_CAND_PREF_SUBEID,
					(s32 *)&op_len, (s32)(subelem_len - 15));

		ptr = (u8 *)(ptr + subelem_len + 2);
		elem_len -= (subelem_len + 2);
		if (ptr + 1 < pend)
			subelem_len = *(ptr + 1);
		if (from_btm) {
			pcandidate = (pcandidates + i);
			memcpy(&pcandidate->nb_rpt, pie, sizeof(*pie));
			if (op && op_len != 0) {
				pcandidate->preference = *(op + 2);
				bfound = _TRUE;
			} else {
				pcandidate->preference = 0;
			}
		} else {
			if (_rtw_memcmp(&pnb->nb_rpt[i], pie, sizeof(*pie)) == _FALSE)
				*nb_rpt_is_same = _FALSE;
			memcpy(&pnb->nb_rpt[i], pie, sizeof(*pie));
		}
		nb_rpt_entries++;
	}

	if (from_btm)
		pnb->preference_en = bfound ? _TRUE : _FALSE;

	*nb_rpt_num = nb_rpt_entries;
	return 0;
}

void host_wnm_reset_btm_candidate(struct roam_nb_info *pnb)
{
	pnb->preference_en = _FALSE;
	memset(pnb->roam_target_addr, 0, ETH_ALEN);
}

void host_wnm_reset_btm_cache(_adapter *padapter)
{
	struct roam_nb_info *pnb = &padapter->mlmepriv.nb_info;
	struct btm_rpt_cache *pcache = &pnb->btm_cache;
	u8 flag = (1 << 0);

	if (host_wnm_btm_candidate_validity(pcache, flag))
		return;

	host_wnm_reset_btm_candidate(pnb);
	memset(pcache, 0, sizeof(*pcache));
	pcache->validity_time = wnm_default_validity_time;
	pcache->disassoc_time = wnm_default_disassoc_time;
}

void host_wnm_reset_btm_state(_adapter *padapter)
{
	struct roam_nb_info *pnb = &padapter->mlmepriv.nb_info;

	pnb->last_nb_rpt_entries = 0;
	pnb->nb_rpt_is_same = _TRUE;
	pnb->disassoc_waiting = -1;
	memset(pnb->nb_rpt, 0, sizeof(pnb->nb_rpt));
	host_wnm_reset_btm_cache(padapter);
}
