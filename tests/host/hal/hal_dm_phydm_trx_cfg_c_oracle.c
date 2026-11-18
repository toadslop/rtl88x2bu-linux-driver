// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-108 — mirrors hal/hal_dm.c rtw_phydm_trx_cfg. */

#include <stddef.h>

#include "host_hal_dm_phydm_trx_cfg_types.h"

#ifdef CONFIG_CTRL_TXSS_BY_TP
void rtw_phydm_trx_cfg(_adapter *adapter, bool tx_1ss)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	u32 txpath = HOST_BB_PATH_AB;
	u32 rxpath = HOST_BB_PATH_AB;
	u32 txpath_1ss = HOST_BB_PATH_A;

	(void)hal_data;
	rtw_hal_get_trx_path(adapter_to_dvobj(adapter), NULL, &txpath, &rxpath);
	txpath = tx_1ss ? HOST_BB_PATH_A : txpath;

	if (phydm_api_trx_mode(adapter_to_phydm(adapter), txpath, rxpath,
			       txpath_1ss) == _FALSE)
		RTW_ERR("%s failed\n", __func__);
}
#endif /* CONFIG_CTRL_TXSS_BY_TP */