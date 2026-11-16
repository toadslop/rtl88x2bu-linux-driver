// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-94 SetHalODMVar core switch cases (hal/hal_dm.c). */

#include <string.h>

#include "host_hal_dm_set_odm_var_core_types.h"

void SetHalODMVar(
	_adapter *Adapter,
	u32 eVariable,
	void *pValue1,
	BOOLEAN bSet)
{
	struct dm_struct *podmpriv = adapter_to_phydm(Adapter);

	switch (eVariable) {
	case HAL_ODM_STA_INFO: {
		struct sta_info *psta = (struct sta_info *)pValue1;

		if (bSet) {
			RTW_INFO("### Set STA_(%d) info ###\n", psta->cmn.mac_id);
			psta->cmn.dm_ctrl = STA_DM_CTRL_ACTIVE;
			phydm_cmn_sta_info_hook(podmpriv, psta->cmn.mac_id,
						&(psta->cmn));
		} else {
			RTW_INFO("### Clean STA_(%d) info ###\n", psta->cmn.mac_id);
			psta->cmn.dm_ctrl = 0;
			phydm_cmn_sta_info_hook(podmpriv, psta->cmn.mac_id, NULL);
		}
	}
	break;
	case HAL_ODM_P2P_STATE:
		odm_cmn_info_update(podmpriv, ODM_CMNINFO_WIFI_DIRECT, bSet);
		break;
	case HAL_ODM_WIFI_DISPLAY_STATE:
		odm_cmn_info_update(podmpriv, ODM_CMNINFO_WIFI_DISPLAY, bSet);
		break;
	case HAL_ODM_REGULATION:
		phydm_adaptivity_info_update(podmpriv,
					     PHYDM_ADAPINFO_DOMAIN_CODE_2G, 0);
		phydm_adaptivity_info_update(podmpriv,
					     PHYDM_ADAPINFO_DOMAIN_CODE_5G, 0);
		break;
	case HAL_ODM_INITIAL_GAIN: {
		u8 rx_gain = *((u8 *)(pValue1));

		if (rx_gain == 0xff) {
			odm_pause_dig(podmpriv, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_0,
				      rx_gain);
		} else {
			odm_pause_dig(podmpriv, PHYDM_PAUSE, PHYDM_PAUSE_LEVEL_0,
				      rx_gain);
		}
	}
	break;
	default:
		break;
	}
}