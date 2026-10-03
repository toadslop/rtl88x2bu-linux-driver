/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_WNM_TYPES_H
#define HOST_WNM_TYPES_H

#include "host_types.h"

#define _TRUE 1
#define _FALSE 0
#define ETH_ALEN 6
#define RTW_MAX_NB_RPT_NUM 8
#define RTW_WLAN_ACTION_WNM_NB_RPT_ELEM 0x34
#define WNM_BTM_CAND_PREF_SUBEID 0x03
#define BSS_TERMINATION_INCLUDED (1 << 3)
#define ESS_DISASSOC_IMMINENT (1 << 4)
#define WNM_BTM_TERM_DUR_SUBEID 0x04

typedef unsigned long systime;

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

struct btm_rpt_cache {
	u8 dialog_token;
	u8 req_mode;
	u16 disassoc_timer;
	u8 validity_interval;
	struct btm_term_duration term_duration;
	u32 validity_time;
	u32 disassoc_time;
	systime req_stime;
};

struct nb_rpt_hdr {
	u8 id;
	u8 len;
	u8 bssid[ETH_ALEN];
	u32 bss_info;
	u8 reg_class;
	u8 ch_num;
	u8 phy_type;
};

struct wnm_btm_cant {
	struct nb_rpt_hdr nb_rpt;
	u8 preference;
};

struct roam_nb_info {
	struct nb_rpt_hdr nb_rpt[RTW_MAX_NB_RPT_NUM];
	struct btm_rpt_cache btm_cache;
	u8 preference_en;
	u8 roam_target_addr[ETH_ALEN];
	u32 last_nb_rpt_entries;
	u8 nb_rpt_is_same;
	s8 disassoc_waiting;
};

struct mlme_priv {
	struct roam_nb_info nb_info;
};

struct _adapter {
	struct mlme_priv mlmepriv;
};

typedef struct _adapter _adapter;

int _rtw_memcmp(const void *s1, const void *s2, size_t n);
u8 *rtw_get_ie(const u8 *pbuf, s32 index, s32 *len, s32 limit);
s32 rtw_get_passing_time_ms(systime start);

void host_wnm_btm_req_hdr_parsing(u8 *pframe, struct btm_req_hdr *phdr);
u32 host_wnm_btm_candidates_offset_get(u8 *pframe);
u8 host_wnm_btm_candidate_validity(struct btm_rpt_cache *pcache, u8 flag);
u32 host_wnm_btm_rsp_candidates_sz_get(_adapter *padapter, u8 *pframe,
				       u32 frame_len);
u8 host_wnm_nb_elem_parsing(u8 *pdata, u32 data_len, u8 from_btm,
			    u32 *nb_rpt_num, u8 *nb_rpt_is_same,
			    struct roam_nb_info *pnb,
			    struct wnm_btm_cant *pcandidates);
void host_wnm_reset_btm_candidate(struct roam_nb_info *pnb);
void host_wnm_reset_btm_cache(_adapter *padapter);
void host_wnm_reset_btm_state(_adapter *padapter);
void host_wnm_set_passing_ms(u32 ms);

#endif /* HOST_WNM_TYPES_H */
