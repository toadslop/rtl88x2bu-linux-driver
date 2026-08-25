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

#ifndef HOST_RF_KFREE_TX_GAIN_TEST
#if defined(CONFIG_RUST) && defined(CONFIG_RUST_RF_KFREE_TX_GAIN)
s8 rtw_rf_get_kfree_tx_gain_offset(_adapter *padapter, u8 path, u8 ch);
#endif
#if !defined(CONFIG_RUST) || !defined(CONFIG_RUST_RF_KFREE_TX_GAIN_SET)
void rtw_rf_set_tx_gain_offset(_adapter *adapter, u8 path, s8 offset)
{
#if !defined(CONFIG_RTL8814A) && !defined(CONFIG_RTL8822B) && !defined(CONFIG_RTL8821C) && !defined(CONFIG_RTL8822C) \
    && !defined(CONFIG_RTL8723F)
	u8 write_value;
#endif
	u8 target_path = 0;
	u32 val32 = 0;

	if (IS_HARDWARE_TYPE_8723D(adapter)) {
		target_path = RF_PATH_A; /*in 8723D case path means S0/S1*/
		if (path == PPG_8723D_S1)
			RTW_INFO("kfree gain_offset 0x55:0x%x ",
			rtw_hal_read_rfreg(adapter, target_path, 0x55, 0xffffffff));
		else if (path == PPG_8723D_S0)
			RTW_INFO("kfree gain_offset 0x65:0x%x ",
			rtw_hal_read_rfreg(adapter, target_path, 0x65, 0xffffffff));
	} else {
		target_path = path;
		RTW_INFO("kfree gain_offset 0x55:0x%x ", rtw_hal_read_rfreg(adapter, target_path, 0x55, 0xffffffff));
	}
	
	switch (rtw_get_chip_type(adapter)) {
#ifdef CONFIG_RTL8723D
	case RTL8723D:
		write_value = RF_TX_GAIN_OFFSET_8723D(offset);
		if (path == PPG_8723D_S1)
			rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0f8000, write_value);
		else if (path == PPG_8723D_S0)
			rtw_hal_write_rfreg(adapter, target_path, 0x65, 0x0f8000, write_value);
		break;
#endif /* CONFIG_RTL8723D */
#ifdef CONFIG_RTL8703B
	case RTL8703B:
		write_value = RF_TX_GAIN_OFFSET_8703B(offset);
		rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0fc000, write_value);
		break;
#endif /* CONFIG_RTL8703B */
#ifdef CONFIG_RTL8188F
	case RTL8188F:
		write_value = RF_TX_GAIN_OFFSET_8188F(offset);
		rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0fc000, write_value);
		break;
#endif /* CONFIG_RTL8188F */
#ifdef CONFIG_RTL8188GTV
	case RTL8188GTV:
		write_value = RF_TX_GAIN_OFFSET_8188GTV(offset);
		rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0fc000, write_value);
		break;
#endif /* CONFIG_RTL8188GTV */
#ifdef CONFIG_RTL8192E
	case RTL8192E:
		write_value = RF_TX_GAIN_OFFSET_8192E(offset);
		rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0f8000, write_value);
		break;
#endif /* CONFIG_RTL8188F */

#ifdef CONFIG_RTL8821A
	case RTL8821:
		write_value = RF_TX_GAIN_OFFSET_8821A(offset);
		rtw_hal_write_rfreg(adapter, target_path, 0x55, 0x0f8000, write_value);
		break;
#endif /* CONFIG_RTL8821A */
#if defined(CONFIG_RTL8814A) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8192F) || defined(CONFIG_RTL8822C) \
    || defined(CONFIG_RTL8723F)
	case RTL8814A:
	case RTL8822B:
	case RTL8822C:	
	case RTL8821C:
	case RTL8192F:
	case RTL8723F:
		RTW_INFO("\nkfree by PhyDM on the sw CH. path %d\n", path);
		break;
#endif /* CONFIG_RTL8814A || CONFIG_RTL8822B || CONFIG_RTL8821C || CONFIG_RTL8723F */

	default:
		rtw_warn_on(1);
		break;
	}
	
	if (IS_HARDWARE_TYPE_8723D(adapter)) {
		if (path == PPG_8723D_S1)
			val32 = rtw_hal_read_rfreg(adapter, target_path, 0x55, 0xffffffff);
		else if (path == PPG_8723D_S0)
			val32 = rtw_hal_read_rfreg(adapter, target_path, 0x65, 0xffffffff);
	} else {
		val32 = rtw_hal_read_rfreg(adapter, target_path, 0x55, 0xffffffff);
	}
	RTW_INFO(" after :0x%x\n", val32);
}

void rtw_rf_apply_tx_gain_offset(_adapter *adapter, u8 ch)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	s8 kfree_offset = 0;
	s8 tx_pwr_track_offset = 0; /* TODO: 8814A should consider tx pwr track when setting tx gain offset */
	s8 total_offset;
	int i, total = 0;

	if (IS_HARDWARE_TYPE_8723D(adapter))
		total = 2; /* S1 and S0 */
	else
		total = hal_spec->rf_reg_path_num;

	for (i = 0; i < total; i++) {
		kfree_offset = rtw_rf_get_kfree_tx_gain_offset(adapter, i, ch);
		total_offset = kfree_offset + tx_pwr_track_offset;
		rtw_rf_set_tx_gain_offset(adapter, i, total_offset);
	}
}
#endif /* !CONFIG_RUST || !CONFIG_RUST_RF_KFREE_TX_GAIN_SET */
#endif /* !HOST_RF_KFREE_TX_GAIN_TEST */

#if defined(CONFIG_RUST) && !defined(HOST_RF_KFREE_TX_GAIN_TEST) && defined(CONFIG_RUST_RF_KFREE_TX_GAIN)

#include <drv_types.h>
#include <hal_data.h>

#ifdef CONFIG_RF_POWER_TRIM
struct kfree_data_t *rtw_rust_kfree_get_kfree_data(_adapter *adapter)
{
	return GET_KFREE_DATA(adapter);
}
#endif /* CONFIG_RF_POWER_TRIM */

void rtw_rust_kfree_warn_on(bool cond)
{
	if (cond)
		rtw_warn_on(1);
}

#endif /* CONFIG_RUST && !HOST_RF_KFREE_TX_GAIN_TEST && CONFIG_RUST_RF_KFREE_TX_GAIN */
