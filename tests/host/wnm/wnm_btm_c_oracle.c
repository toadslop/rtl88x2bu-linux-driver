// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_wnm_types.h"

#define ETH_ALEN 6

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
