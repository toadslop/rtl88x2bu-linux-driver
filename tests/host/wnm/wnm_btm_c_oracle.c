// SPDX-License-Identifier: GPL-2.0
#include <string.h>
#include "host_wnm_types.h"

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
