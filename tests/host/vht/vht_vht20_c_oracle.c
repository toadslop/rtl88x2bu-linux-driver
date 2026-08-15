// SPDX-License-Identifier: GPL-2.0
/* C oracle slice: rtw_check_for_vht20 from core/rtw_vht.c (W3-45). */

#include "host_vht_mcs_types.h"

void rtw_check_for_vht20(_adapter *adapter, u8 *ies, int ies_len)
{
	u8 ht_ch, ht_bw, ht_offset;
	u8 vht_ch, vht_bw, vht_offset;

	rtw_ies_get_chbw(ies, ies_len, &ht_ch, &ht_bw, &ht_offset, 1, 0);
	rtw_ies_get_chbw(ies, ies_len, &vht_ch, &vht_bw, &vht_offset, 1, 1);

	if (ht_bw == CHANNEL_WIDTH_20 && vht_bw >= CHANNEL_WIDTH_80) {
		u8 *vht_op_ie;
		int vht_op_ielen;

		RTW_INFO(FUNC_ADPT_FMT " vht80 is not allowed without ht40\n",
			 FUNC_ADPT_ARG(adapter));
		vht_op_ie = rtw_get_ie(ies, EID_VHTOperation, &vht_op_ielen, ies_len);
		if (vht_op_ie && vht_op_ielen) {
			RTW_INFO(FUNC_ADPT_FMT " switch to vht20\n",
				 FUNC_ADPT_ARG(adapter));
			SET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2, 0);
			SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(vht_op_ie + 2, 0);
			SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(vht_op_ie + 2, 0);
		}
	}
}
