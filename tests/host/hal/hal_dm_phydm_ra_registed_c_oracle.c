// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-107 — mirrors hal/hal_dm.c rtw_phydm_ra_registed. */

#include <stdint.h>

#include "host_hal_dm_phydm_ra_registed_types.h"

void rtw_phydm_ra_registed(_adapter *adapter, struct sta_info *psta)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);

	if (psta == NULL) {
		RTW_ERR(FUNC_ADPT_FMT " sta is NULL\n", FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		return;
	}

	if (psta->cmn.mac_id >= macid_ctl->num)
		return;

	phydm_ra_registed(&hal_data->odmpriv, psta->cmn.mac_id,
			  (u8)psta->cmn.rssi_stat.rssi);
	dump_sta_info((void *)(uintptr_t)RTW_DBGDUMP, psta);
}