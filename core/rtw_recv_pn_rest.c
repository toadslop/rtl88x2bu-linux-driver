// SPDX-License-Identifier: GPL-2.0
#define _RTW_RECV_PN_REST_C_

#ifdef HOST_RECV_PN_TEST
#include "host_recv_types.h"
#define le64_to_cpu host_le64_to_cpu
#define cpu_to_le64 host_cpu_to_le64
#define IS_MCAST host_is_mcast
#else
#include <drv_types.h>
#endif

#ifdef HOST_RECV_PN_TEST
typedef unsigned int uint;
#endif

#define PN_LESS_CHK(a, b)	(((a - b) & 0x800000000000) != 0)
#define VALID_PN_CHK(new, old)	(((old) == 0) || PN_LESS_CHK(old, new))
#ifndef HOST_RECV_PN_TEST
#define CCMPH_2_KEYID(ch)	(((ch) & 0x00000000c0000000) >> 30)
#endif

sint recv_ucast_pn_decache(union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info *sta = precv_frame->u.hdr.psta;
	struct stainfo_rxcache *prxcache = &sta->sta_recvpriv.rxcache;
	u8 *pdata = precv_frame->u.hdr.rx_data;
	sint tid = precv_frame->u.hdr.attrib.priority;
	u64 tmp_iv_hdr = 0;
	u64 curr_pn = 0, pkt_pn = 0;

	if (tid > 15)
		return _FAIL;

	if (pattrib->encrypt == _AES_) {
		tmp_iv_hdr = le64_to_cpu(*(u64 *)(pdata + pattrib->hdrlen));
		pkt_pn = CCMPH_2_PN(tmp_iv_hdr);
		tmp_iv_hdr = le64_to_cpu(*(u64 *)prxcache->iv[tid]);
		curr_pn = CCMPH_2_PN(tmp_iv_hdr);

		if (!VALID_PN_CHK(pkt_pn, curr_pn)) {
			/* return _FAIL; */
		} else {
			prxcache->last_tid = tid;
			_rtw_memcpy(prxcache->iv[tid],
				    (pdata + pattrib->hdrlen),
				    sizeof(prxcache->iv[tid]));
		}
	}

	return _SUCCESS;
}

sint recv_bcast_pn_decache(union recv_frame *precv_frame)
{
	_adapter *padapter = precv_frame->u.hdr.adapter;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	u8 *pdata = precv_frame->u.hdr.rx_data;
	u64 tmp_iv_hdr = 0;
	u64 curr_pn = 0, pkt_pn = 0;
	u8 key_id;

	if ((pattrib->encrypt == _AES_) &&
	    (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE)) {

		tmp_iv_hdr = le64_to_cpu(*(u64 *)(pdata + pattrib->hdrlen));
		key_id = CCMPH_2_KEYID(tmp_iv_hdr);
		pkt_pn = CCMPH_2_PN(tmp_iv_hdr);

		curr_pn = le64_to_cpu(*(u64 *)psecuritypriv->iv_seq[key_id]);
		curr_pn &= 0x0000ffffffffffff;

		if (!VALID_PN_CHK(pkt_pn, curr_pn))
			return _FAIL;

		*(u64 *)psecuritypriv->iv_seq[key_id] = cpu_to_le64(pkt_pn);
	}

	return _SUCCESS;
}

sint recv_decache(union recv_frame *precv_frame)
{
	struct sta_info *psta = precv_frame->u.hdr.psta;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	sint tid = pattrib->priority;
	u16 seq_ctrl = ((precv_frame->u.hdr.attrib.seq_num & 0xffff) << 4) |
		       (precv_frame->u.hdr.attrib.frag_num & 0xf);
	u16 *prxseq;

	if (tid > 15)
		return _FAIL;

	if (pattrib->qos) {
		if (IS_MCAST(pattrib->ra))
			prxseq = &psta->sta_recvpriv.bmc_tid_rxseq[tid];
		else
			prxseq = &psta->sta_recvpriv.rxcache.tid_rxseq[tid];
	} else {
		if (IS_MCAST(pattrib->ra))
			prxseq = &psta->sta_recvpriv.nonqos_bmc_rxseq;
		else
			prxseq = &psta->sta_recvpriv.nonqos_rxseq;
	}

	if (seq_ctrl == *prxseq) {
		psta->sta_stats.duplicate_cnt++;
		#ifdef DBG_RX_DROP_FRAME
		RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" recv_decache _FAIL for sta="MAC_FMT"\n"
			, FUNC_ADPT_ARG(psta->padapter), MAC_ARG(psta->cmn.mac_addr));
		#endif
		return _FAIL;
	}
	*prxseq = seq_ctrl;

	return _SUCCESS;
}
