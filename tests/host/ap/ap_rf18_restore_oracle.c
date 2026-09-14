// SPDX-License-Identifier: GPL-2.0
/* Host oracle for rtw_check_restore_rf18 (W3-82 PR13 L2). */
#include "host_ap_rf18_restore_types.h"

void rtw_check_restore_rf18(_adapter *padapter)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	u32 reg;
	u8 union_ch = 0, union_bw = 0, union_offset = 0, setchbw = _FALSE;

	reg = rtw_hal_read_rfreg(padapter, 0, 0x18, 0x3FF);
	if ((reg & 0xFF) == 0)
		setchbw = _TRUE;
	reg = rtw_hal_read_rfreg(padapter, 1, 0x18, 0x3FF);
	if ((reg & 0xFF) == 0)
		setchbw = _TRUE;

	if (setchbw) {
		if (!rtw_mi_get_ch_setting_union(padapter, &union_ch, &union_bw, &union_offset)) {
			union_ch = pmlmeext->cur_channel;
			union_offset = pmlmeext->cur_ch_offset;
			union_bw = pmlmeext->cur_bwmode;
		}
		padapter->hal_data.current_channel = 0;
		set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
	}
}
