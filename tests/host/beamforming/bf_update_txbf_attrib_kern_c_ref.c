// SPDX-License-Identifier: GPL-2.0
/* L1 C reference for W3-197 legacy update_attrib_txbf_info (host oracle wrapper). */

#include "host_beamforming_bf_update_txbf_attrib_oracle.h"

void update_attrib_txbf_info(bf_host_cmd_traffic_padpt padapter, struct bf_host_pkt_attrib *pattrib,
			     struct bf_host_sta *psta)
{
	o_update_attrib_txbf_info(padapter, pattrib, psta);
}
