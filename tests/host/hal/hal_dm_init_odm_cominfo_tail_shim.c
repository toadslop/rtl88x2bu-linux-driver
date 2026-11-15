// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-88 Init_ODM ComInfo tail L2 test. */

#include <stddef.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

extern struct host_hal_dm_init_odm_tail_trace host_hal_dm_init_odm_tail_trace;

struct rtw_hal_dm_init_odm_tail_hooks {
	u8 *current_channel;
	u8 *b_scan_in_process;
	u8 *bpower_saving;
	u32 *cur_tx_tp;
	u32 *cur_rx_tp;
	u8 *antenna_test;
	u8 *multi_ch_switch_mode;
	u32 *halrf_ability;
};

void rtw_rust_hal_dm_init_odm_tail_optional_hooks(struct _adapter *adapter, struct dm_struct *phydm)
{
	(void)adapter;
	(void)phydm;
	odm_cmn_info_hook(phydm, 31, &adapter->dvobj.usb_speed);
}

static struct pwrctrl_priv host_pwrctl;

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(struct _adapter *adapter)
{
	return adapter ? &GET_HAL_DATA(adapter)->odmpriv : NULL;
}

void rtw_rust_hal_dm_init_odm_tail_fill_hooks(struct _adapter *adapter,
	struct rtw_hal_dm_init_odm_tail_hooks *out)
{
	struct dvobj_priv *dvobj;

	if (!adapter || !out)
		return;
	dvobj = &adapter->dvobj;
	out->current_channel = &GET_HAL_DATA(adapter)->current_channel;
	out->b_scan_in_process = &GET_HAL_DATA(adapter)->b_scan_in_process;
	out->bpower_saving = &host_pwrctl.bpower_saving;
	out->cur_tx_tp = &dvobj->traffic_stat.cur_tx_tp;
	out->cur_rx_tp = &dvobj->traffic_stat.cur_rx_tp;
	out->antenna_test = &GET_HAL_DATA(adapter)->antenna_test;
	out->multi_ch_switch_mode = &GET_HAL_DATA(adapter)->multi_ch_switch_mode;
	out->halrf_ability = &adapter->registrypriv.halrf_ability;
}

void rtw_rust_hal_dm_init_odm_tail_sta_hooks(struct dm_struct *phydm)
{
	(void)phydm;
	host_hal_dm_init_odm_tail_trace.sta_hooks++;
}

void rtw_rust_hal_dm_init_odm_tail_finish(struct dm_struct *phydm)
{
	(void)phydm;
	host_hal_dm_init_odm_tail_trace.finish++;
}