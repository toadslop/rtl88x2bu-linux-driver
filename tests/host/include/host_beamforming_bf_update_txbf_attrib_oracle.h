/* SPDX-License-Identifier: GPL-2.0 */
#ifndef HOST_BEAMFORMING_BF_UPDATE_TXBF_ATTRIB_ORACLE_H
#define HOST_BEAMFORMING_BF_UPDATE_TXBF_ATTRIB_ORACLE_H

#include "host_beamforming_bf_cmd_traffic_oracle.h"

static inline void o_update_attrib_txbf_info(bf_host_cmd_traffic_padpt padapter,
					     struct bf_host_pkt_attrib *pattrib,
					     struct bf_host_sta *psta)
{
	(void)padapter;
	if (psta) {
		pattrib->txbf_g_id = psta->cmn.g_id;
		pattrib->txbf_p_aid = psta->cmn.p_aid;
	}
}

#endif
