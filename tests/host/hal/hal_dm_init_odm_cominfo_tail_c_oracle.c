// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-88 Init_ODM ComInfo tail — mirrors hal/hal_dm.c (8822B USB). */

#include <stddef.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

#define ODM_CMNINFO_HUBUSBMODE 31
#define ODM_CMNINFO_TX_TP 33
#define ODM_CMNINFO_RX_TP 34
#define ODM_CMNINFO_CHNL 58
#define ODM_CMNINFO_SCAN 64
#define ODM_CMNINFO_POWER_SAVING 65
#define ODM_CMNINFO_ANT_TEST 70
#define ODM_CMNINFO_FCS_MODE 73
#define HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY 4

static struct dm_struct *adapter_to_phydm(struct _adapter *adapter)
{
	return adapter ? &GET_HAL_DATA(adapter)->odmpriv : NULL;
}

void rtw_rust_hal_dm_init_odm_cominfo_tail(struct _adapter *adapter)
{
	struct dm_struct *phydm;
	struct hal_data *pHalData;
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrctl;
	int i;

	if (!adapter)
		return;
	phydm = adapter_to_phydm(adapter);
	if (!phydm)
		return;

	pHalData = GET_HAL_DATA(adapter);
	dvobj = &adapter->dvobj;
	pwrctl = adapter_to_pwrctl(adapter);
	if (!pwrctl)
		return;

	odm_cmn_info_hook(phydm, ODM_CMNINFO_CHNL, &pHalData->current_channel);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_SCAN, &pHalData->b_scan_in_process);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_POWER_SAVING, &pwrctl->bpower_saving);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_TX_TP, &dvobj->traffic_stat.cur_tx_tp);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_RX_TP, &dvobj->traffic_stat.cur_rx_tp);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_ANT_TEST, &pHalData->antenna_test);
#ifdef CONFIG_USB_HCI
	odm_cmn_info_hook(phydm, ODM_CMNINFO_HUBUSBMODE, &dvobj->usb_speed);
#endif
	odm_cmn_info_hook(phydm, ODM_CMNINFO_FCS_MODE, &pHalData->multi_ch_switch_mode);
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY,
		&adapter->registrypriv.halrf_ability);

	for (i = 0; i < HOST_HAL_ODM_ASSOCIATE_ENTRY_NUM; i++)
		phydm_cmn_sta_info_hook(phydm, (u8)i, NULL);

	rtw_phydm_ops_func_init(phydm);
	phydm_dm_early_init(phydm);
}