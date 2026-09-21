/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_WNM_TYPES_H
#define HOST_WNM_TYPES_H

#include "host_types.h"

#define BSS_TERMINATION_INCLUDED (1 << 3)
#define ESS_DISASSOC_IMMINENT (1 << 4)
#define WNM_BTM_TERM_DUR_SUBEID 0x04

struct btm_term_duration {
	u8 id;
	u8 len;
	u64 tsf;
	u16 duration;
};

struct btm_req_hdr {
	u8 dialog_token;
	u8 req_mode;
	u16 disassoc_timer;
	u8 validity_interval;
	struct btm_term_duration term_duration;
};

void host_wnm_btm_req_hdr_parsing(u8 *pframe, struct btm_req_hdr *phdr);
u32 host_wnm_btm_candidates_offset_get(u8 *pframe);

#endif /* HOST_WNM_TYPES_H */
