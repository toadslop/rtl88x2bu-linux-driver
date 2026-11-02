// SPDX-License-Identifier: GPL-2.0
/* L1 C reference for W3-201 txvector + STA beamform cap exports. */

#include "host_beamforming_bf_txvector_cap_oracle.h"

#define BF_TXVECTOR_CAP_KERN_REF 1
#include "bf_txvector_cap_oracle.c"

void rtw_rust_get_txvector_parameter(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				     u8 *g_id, u16 *p_aid)
{
	o_rtw_bf_get_txvector_parameter(adapter, sta, g_id, p_aid);
}

void rtw_rust_get_sta_beamform_cap(bf_host_txvec_padpt adapter, struct bf_host_txvec_sta *sta,
				   u8 *sta_bf_cap, u8 *sounding_dim, u8 *comp_steering)
{
	o_rtw_bf_get_sta_beamform_cap(adapter, sta, sta_bf_cap, sounding_dim, comp_steering);
}
