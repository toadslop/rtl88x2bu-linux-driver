// SPDX-License-Identifier: GPL-2.0
/* W3-197: C helper for rust/rtw_beamforming.rs legacy update_attrib_txbf_info. */

#include <drv_types.h>

#if defined(CONFIG_RUST) && defined(CONFIG_BEAMFORMING) && !defined(RTW_BEAMFORMING_VERSION_2)

void rtw_rust_update_attrib_txbf_info_impl(_adapter *padapter, struct pkt_attrib *pattrib,
					   struct sta_info *psta)
{
	(void)padapter;
	if (psta) {
		pattrib->txbf_g_id = psta->cmn.bf_info.g_id;
		pattrib->txbf_p_aid = psta->cmn.bf_info.p_aid;
	}
}

#endif
