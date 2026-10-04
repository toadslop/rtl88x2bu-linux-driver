// SPDX-License-Identifier: GPL-2.0
/* Radio measurement subelement parse helpers (W3-111). */
#define _RTW_RM_PARSE_REST_C_

#ifdef HOST_RM_PARSE_TEST
#include "host_rm_parse_types.h"
#else
#include <drv_types.h>
#endif

#ifdef CONFIG_RTW_80211K

#if !defined(CONFIG_RUST) || defined(HOST_RM_PARSE_TEST)

int rm_en_cap_chk_and_set(struct rm_obj *prm, enum rm_cap_en en);

int rm_parse_ch_load_s_elem(struct rm_obj *prm, u8 *pbody, int req_len)
{
	int p = 0;
	int len = req_len;

	prm->q.opt_s_elem_len = len;
	while (len) {
		switch (pbody[p]) {
		case ch_load_rep_info:
			rm_en_cap_chk_and_set(prm, RM_CH_LOAD_CAP_EN);
			_rtw_memcpy(&(prm->q.opt.clm.rep_cond), &pbody[p + 2],
				    sizeof(prm->q.opt.clm.rep_cond));
			break;
		default:
			break;
		}
		len = len - (int)pbody[p + 1] - 2;
		p = p + (int)pbody[p + 1] + 2;
	}
	return _SUCCESS;
}

int rm_parse_noise_histo_s_elem(struct rm_obj *prm, u8 *pbody, int req_len)
{
	int p = 0;
	int len = req_len;

	prm->q.opt_s_elem_len = len;
	while (len) {
		switch (pbody[p]) {
		case noise_histo_rep_info:
			rm_en_cap_chk_and_set(prm, RM_NOISE_HISTO_CAP_EN);
			_rtw_memcpy(&(prm->q.opt.nhm.rep_cond), &pbody[p + 2],
				    sizeof(prm->q.opt.nhm.rep_cond));
			break;
		default:
			break;
		}
		len = len - (int)pbody[p + 1] - 2;
		p = p + (int)pbody[p + 1] + 2;
	}
	return _SUCCESS;
}

#endif /* !CONFIG_RUST || HOST_RM_PARSE_TEST */
#endif /* CONFIG_RTW_80211K */
