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
u8 rm_get_bcn_rcpi(struct rm_obj *prm, struct wlan_network *pnetwork);
u8 rm_get_bcn_rsni(struct rm_obj *prm, struct wlan_network *pnetwork);

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

int rm_parse_bcn_req_s_elem(struct rm_obj *prm, u8 *pbody, int req_len)
{
	int i, p = 0;
	int len = req_len;
	int ap_ch_rpt_idx = 0;
	struct _RT_OPERATING_CLASS *op;
	u8 *popt_id;

	prm->q.opt_s_elem_len = len;
	popt_id = prm->q.opt.bcn.opt_id;
	while (len && prm->q.opt.bcn.opt_id_num < BCN_REQ_OPT_MAX_NUM) {
		switch (pbody[p]) {
		case bcn_req_ssid:
			prm->q.opt.bcn.ssid.SsidLength = pbody[p + 1];
			_rtw_memcpy(&(prm->q.opt.bcn.ssid.Ssid), &pbody[p + 2],
				    MIN(pbody[p + 1],
					sizeof(prm->q.opt.bcn.ssid.Ssid) - 1));
			popt_id[prm->q.opt.bcn.opt_id_num++] = pbody[p];
			break;
		case bcn_req_rep_info:
			rm_en_cap_chk_and_set(prm, RM_BCN_MEAS_REP_COND_CAP_EN);
			_rtw_memcpy(&(prm->q.opt.bcn.rep_cond), &pbody[p + 2],
				    sizeof(prm->q.opt.bcn.rep_cond));
			break;
		case bcn_req_rep_detail:
			prm->q.opt.bcn.rep_detail = pbody[p + 2];
			popt_id[prm->q.opt.bcn.opt_id_num++] = pbody[p];
			break;
		case bcn_req_req:
			prm->q.opt.bcn.req_start = rtw_malloc(pbody[p + 1]);
			if (prm->q.opt.bcn.req_start == NULL)
				break;
			for (i = 0; i < pbody[p + 1]; i++)
				*((prm->q.opt.bcn.req_start) + i) = pbody[p + 2 + i];
			prm->q.opt.bcn.req_len = pbody[p + 1];
			popt_id[prm->q.opt.bcn.opt_id_num++] = pbody[p];
			break;
		case bcn_req_ap_ch_rep:
			if (ap_ch_rpt_idx > BCN_REQ_OPT_AP_CH_RPT_MAX_NUM)
				break;
			popt_id[prm->q.opt.bcn.opt_id_num++] = pbody[p];
			op = rtw_malloc(sizeof(*op));
			if (!op)
				break;
			op->global_op_class = pbody[p + 2];
			i = pbody[p + 1] - 1;
			op->Len = i;
			memcpy(op->Channel, &pbody[p + 3], MIN(i, MAX_CH_NUM_IN_OP_CLASS));
			prm->q.opt.bcn.ap_ch_rpt[ap_ch_rpt_idx++] = op;
			prm->q.opt.bcn.ap_ch_rpt_num = ap_ch_rpt_idx;
			break;
		default:
			break;
		}
		len = len - (int)pbody[p + 1] - 2;
		p = p + (int)pbody[p + 1] + 2;
	}
	return _SUCCESS;
}

int rm_parse_meas_req(struct rm_obj *prm, u8 *pbody)
{
	int p;
	int req_len;

	req_len = (int)pbody[1];
	p = 5;

	prm->q.op_class = pbody[p++];
	prm->q.ch_num = pbody[p++];
	prm->q.rand_intvl = le16_to_cpu(*(u16 *)(&pbody[p]));
	p += 2;
	prm->q.meas_dur = le16_to_cpu(*(u16 *)(&pbody[p]));
	p += 2;

	if (prm->q.m_type == bcn_req) {
		prm->q.m_mode = pbody[p++];
		_rtw_memcpy(&(prm->q.bssid), &pbody[p], 6);
		p += 6;
		prm->q.opt.bcn.rep_detail = 2;
	}

	if (req_len - (p - 2) <= 0)
		return _SUCCESS;

	switch (prm->q.m_type) {
	case bcn_req:
		rm_parse_bcn_req_s_elem(prm, &pbody[p], req_len - (p - 2));
		break;
	case ch_load_req:
		rm_parse_ch_load_s_elem(prm, &pbody[p], req_len - (p - 2));
		break;
	case noise_histo_req:
		rm_parse_noise_histo_s_elem(prm, &pbody[p], req_len - (p - 2));
		break;
	default:
		break;
	}

	return _SUCCESS;
}

u8 rm_bcn_req_cond_mach(struct rm_obj *prm, struct wlan_network *pnetwork)
{
	u8 val8;

	switch (prm->q.opt.bcn.rep_cond.cond) {
	case bcn_rep_cond_immediately:
		return _SUCCESS;
	case bcn_req_cond_rcpi_greater:
		val8 = rm_get_bcn_rcpi(prm, pnetwork);
		if (val8 > prm->q.opt.bcn.rep_cond.threshold)
			return _SUCCESS;
		break;
	case bcn_req_cond_rcpi_less:
		val8 = rm_get_bcn_rcpi(prm, pnetwork);
		if (val8 < prm->q.opt.bcn.rep_cond.threshold)
			return _SUCCESS;
		break;
	case bcn_req_cond_rsni_greater:
		val8 = rm_get_bcn_rsni(prm, pnetwork);
		if (val8 != 255 && val8 > prm->q.opt.bcn.rep_cond.threshold)
			return _SUCCESS;
		break;
	case bcn_req_cond_rsni_less:
		val8 = rm_get_bcn_rsni(prm, pnetwork);
		if (val8 != 255 && val8 < prm->q.opt.bcn.rep_cond.threshold)
			return _SUCCESS;
		break;
	default:
		RTW_ERR("RM: bcn_req cond %u not support\n",
			prm->q.opt.bcn.rep_cond.cond);
		break;
	}
	return _FALSE;
}

#endif /* !CONFIG_RUST || HOST_RM_PARSE_TEST */
#endif /* CONFIG_RTW_80211K */
