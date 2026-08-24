/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#define _RTW_RF_KFREE_TX_GAIN_C_

#ifdef HOST_RF_KFREE_TX_GAIN_TEST
#include "host_rf_kfree_tx_gain_types.h"
#else
#include <drv_types.h>
#include <hal_data.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_RF_KFREE_TX_GAIN_TEST) || !defined(CONFIG_RUST_RF_KFREE_TX_GAIN)
s8 rtw_rf_get_kfree_tx_gain_offset(_adapter *padapter, u8 path, u8 ch)
{
	s8 kfree_offset = 0;

#ifdef CONFIG_RF_POWER_TRIM
	struct kfree_data_t *kfree_data = GET_KFREE_DATA(padapter);
	s8 bb_gain_sel = rtw_ch_to_bb_gain_sel(ch);

	if (bb_gain_sel < BB_GAIN_2G || bb_gain_sel >= BB_GAIN_NUM) {
		rtw_warn_on(1);
		goto exit;
	}

	if (kfree_data->flag & KFREE_FLAG_ON) {
		kfree_offset = kfree_data->bb_gain[bb_gain_sel][path];
		if (IS_HARDWARE_TYPE_8723D(padapter))
			RTW_INFO("%s path:%s, ch:%u, bb_gain_sel:%d, kfree_offset:%d\n"
				, __func__, (path == 0)?"S1":"S0",
				ch, bb_gain_sel, kfree_offset);
		else
			RTW_INFO("%s path:%u, ch:%u, bb_gain_sel:%d, kfree_offset:%d\n"
				, __func__, path, ch, bb_gain_sel, kfree_offset);
	}
exit:
#endif /* CONFIG_RF_POWER_TRIM */
	return kfree_offset;
}
#endif /* !CONFIG_RUST || HOST_RF_KFREE_TX_GAIN_TEST || !CONFIG_RUST_RF_KFREE_TX_GAIN */
