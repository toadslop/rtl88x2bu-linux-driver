// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-88 Init_ODM ComInfo tail (channel/scan/USB hooks). */

#include <stddef.h>
#include "host_hal_dm_init_odm_cominfo_tail_types.h"

#define ODM_CMNINFO_CHNL 58
#define ODM_CMNINFO_SCAN 64
#define ODM_CMNINFO_POWER_SAVING 65
#define ODM_CMNINFO_HUBUSBMODE 31
#define ODM_CMNINFO_FCS_MODE 73

#define HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY 4

static struct dm_struct *adapter_to_phydm(struct _adapter *adapter)
{
	return adapter ? &GET_HAL_DATA(adapter)->odmpriv : NULL;
}

void rtw_rust_hal_dm_init_odm_cominfo_tail(struct _adapter *adapter)
{
	struct dm_struct *phydm;
	struct dvobj_priv *dvobj;

	if (!adapter)
		return;
	phydm = adapter_to_phydm(adapter);
	if (!phydm)
		return;
	static struct pwrctrl_priv pwrctl;

	dvobj = &adapter->dvobj;

	odm_cmn_info_hook(phydm, ODM_CMNINFO_CHNL, &GET_HAL_DATA(adapter)->current_channel);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_SCAN, &GET_HAL_DATA(adapter)->b_scan_in_process);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_POWER_SAVING, &pwrctl.bpower_saving);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_HUBUSBMODE, &dvobj->usb_speed);
	odm_cmn_info_hook(phydm, ODM_CMNINFO_FCS_MODE, &GET_HAL_DATA(adapter)->multi_ch_switch_mode);
	halrf_cmn_info_hook(phydm, HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY,
		&adapter->registrypriv.halrf_ability);
	host_hal_dm_init_odm_tail_trace.sta_hooks++;
	host_hal_dm_init_odm_tail_trace.finish++;
}