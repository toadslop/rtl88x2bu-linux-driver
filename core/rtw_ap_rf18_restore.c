/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/
#define _RTW_AP_RF18_RESTORE_C_

#ifdef HOST_AP_RF18_RESTORE_TEST
#include "host_ap_rf18_restore_types.h"
#else
#include <drv_types.h>
#endif

#ifdef RTW_CONFIG_RFREG18_WA

#if !defined(CONFIG_RUST_AP_RF18_RESTORE) || defined(HOST_AP_RF18_RESTORE_TEST)

void rtw_check_restore_rf18(_adapter *padapter)
{
#ifdef HOST_AP_RF18_RESTORE_TEST
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
#else
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
#endif
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
#ifndef HOST_AP_RF18_RESTORE_TEST
			RTW_INFO("Hit RF(0x18)=0!! restore original channel setting.\n");
#endif
			union_ch = pmlmeext->cur_channel;
			union_offset = pmlmeext->cur_ch_offset;
			union_bw = pmlmeext->cur_bwmode;
		}
#ifndef HOST_AP_RF18_RESTORE_TEST
		else {
			RTW_INFO("Hit RF(0x18)=0!! set ch(%x) offset(%x) bwmode(%x)\n", union_ch,
				 union_offset, union_bw);
		}
#endif
#ifdef HOST_AP_RF18_RESTORE_TEST
		padapter->hal_data.current_channel = 0;
#else
		pHalData->current_channel = 0;
#endif
		set_channel_bwmode(padapter, union_ch, union_offset, union_bw);
	}
}

#endif /* !CONFIG_RUST_AP_RF18_RESTORE || HOST_AP_RF18_RESTORE_TEST */

#endif /* RTW_CONFIG_RFREG18_WA */
