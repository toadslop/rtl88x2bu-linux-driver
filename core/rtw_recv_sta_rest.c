/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_RECV_STA_REST_C_

#ifdef HOST_RECV_STA_TEST
#include "host_recv_sta_types.h"
#else
#include <drv_types.h>
#endif

#if !defined(CONFIG_RUST_RECV_STA) || defined(HOST_RECV_STA_TEST)

void count_rx_stats(_adapter *padapter, union recv_frame *prframe, struct sta_info *sta)
{
	int	sz;
	struct sta_info		*psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct rx_pkt_attrib	*pattrib = &prframe->u.hdr.attrib;
	struct recv_priv		*precvpriv = &padapter->recvpriv;

	sz = get_recvframe_len(prframe);
	precvpriv->rx_bytes += sz;

	padapter->mlmepriv.LinkDetectInfo.NumRxOkInPeriod++;

	if ((!MacAddr_isBcst(pattrib->dst)) && (!IS_MCAST(pattrib->dst)))
		padapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod++;

	if (sta)
		psta = sta;
	else
		psta = prframe->u.hdr.psta;

	if (psta) {
		u8 is_ra_bmc = IS_MCAST(pattrib->ra);

		pstats = &psta->sta_stats;

		pstats->last_rx_time = rtw_get_current_time();
		pstats->rx_data_pkts++;
		pstats->rx_bytes += sz;
		if (is_broadcast_mac_addr(pattrib->ra)) {
			pstats->rx_data_bc_pkts++;
			pstats->rx_bc_bytes += sz;
		} else if (is_ra_bmc) {
			pstats->rx_data_mc_pkts++;
			pstats->rx_mc_bytes += sz;
		}

		if (!is_ra_bmc) {
			pstats->rxratecnt[pattrib->data_rate]++;
			/*record rx packets for every tid*/
			pstats->rx_data_qos_pkts[pattrib->priority]++;
		}
#ifdef CONFIG_DYNAMIC_SOML
		rtw_dyn_soml_byte_update(padapter, pattrib->data_rate, sz);
#endif
#if defined(CONFIG_CHECK_LEAVE_LPS) && defined(CONFIG_LPS_CHK_BY_TP)
		if (adapter_to_pwrctl(padapter)->lps_chk_by_tp)
			traffic_check_for_leave_lps_by_tp(padapter, _FALSE, psta);
#endif /* CONFIG_LPS */

	}

#ifdef CONFIG_CHECK_LEAVE_LPS
#ifdef CONFIG_LPS_CHK_BY_TP
	if (!adapter_to_pwrctl(padapter)->lps_chk_by_tp)
#endif
		traffic_check_for_leave_lps(padapter, _FALSE, 0);
#endif /* CONFIG_CHECK_LEAVE_LPS */

}

#endif /* !CONFIG_RUST_RECV_STA || HOST_RECV_STA_TEST */
