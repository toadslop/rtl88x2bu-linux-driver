// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-88 Init_ODM ComInfo tail L2 test. */

#include <stddef.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

struct rtw_hal_dm_init_odm_tail_hooks {
	u8 *current_channel;
	u8 *b_scan_in_process;
	u8 *bpower_saving;
	u32 *cur_tx_tp;
	u32 *cur_rx_tp;
	u8 *antenna_test;
	u8 *usb_speed;
	u8 *multi_ch_switch_mode;
	u32 *halrf_ability;
};

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(struct _adapter *adapter)
{
	return adapter ? &GET_HAL_DATA(adapter)->odmpriv : NULL;
}

void rtw_rust_hal_dm_init_odm_tail_optional_hooks(struct _adapter *adapter,
	struct dm_struct *phydm)
{
	(void)adapter;
	(void)phydm;
}

void rtw_rust_hal_dm_init_odm_tail_fill_hooks(struct _adapter *adapter,
	struct rtw_hal_dm_init_odm_tail_hooks *out)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrctl;

	if (!adapter || !out)
		return;
	dvobj = &adapter->dvobj;
	pwrctl = adapter_to_pwrctl(adapter);
	if (!pwrctl)
		return;

	out->current_channel = &GET_HAL_DATA(adapter)->current_channel;
	out->b_scan_in_process = &GET_HAL_DATA(adapter)->b_scan_in_process;
	out->bpower_saving = &pwrctl->bpower_saving;
	out->cur_tx_tp = &dvobj->traffic_stat.cur_tx_tp;
	out->cur_rx_tp = &dvobj->traffic_stat.cur_rx_tp;
	out->antenna_test = &GET_HAL_DATA(adapter)->antenna_test;
	out->usb_speed = &dvobj->usb_speed;
	out->multi_ch_switch_mode = &GET_HAL_DATA(adapter)->multi_ch_switch_mode;
	out->halrf_ability = &adapter->registrypriv.halrf_ability;
}

void rtw_rust_hal_dm_init_odm_tail_sta_hooks(struct dm_struct *phydm)
{
	int i;

	for (i = 0; i < HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM; i++)
		phydm_cmn_sta_info_hook(phydm, (u8)i, NULL);
}

void rtw_rust_hal_dm_init_odm_tail_finish(struct dm_struct *phydm)
{
	rtw_phydm_ops_func_init(phydm);
	phydm_dm_early_init(phydm);
}