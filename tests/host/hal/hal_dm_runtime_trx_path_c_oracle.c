// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-103 — mirrors hal/hal_dm.c rtw_hal_runtime_trx_path_decision. */

#include <stddef.h>

#include "host_hal_dm_runtime_trx_path_types.h"

u8 rtw_hal_runtime_trx_path_decision(_adapter *adapter)
{
	struct hal_data_host *hal_data = GET_HAL_DATA(adapter);
	u32 txpath;
	u32 rxpath;
	int i;
	u8 rst = _FAIL;

	rtw_hal_get_trx_path(adapter_to_dvobj(adapter), NULL, &txpath, &rxpath);
	if (!txpath)
		goto exit;
	if (!rxpath)
		goto exit;

	tx_path_nss_set_default(hal_data->txpath_nss, hal_data->txpath_num_nss,
			      GET_HAL_TX_PATH_BMP(adapter));

#if defined(CONFIG_RTL8192F) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8822C)
{
	u32 txpath_1ss;

	if (txpath == HOST_BB_PATH_AB) {
		switch (hal_data->max_tx_cnt) {
		case 2:
#ifdef CONFIG_RTW_TX_NPATH_EN
			if (adapter->registrypriv.tx_npath == 1)
				txpath_1ss = HOST_BB_PATH_AB;
			else
#endif
#ifdef CONFIG_RTW_PATH_DIV
			if (adapter->registrypriv.path_div == 1)
				txpath_1ss = HOST_BB_PATH_AUTO;
			else
#endif
				txpath_1ss = HOST_BB_PATH_A;
			break;
		case 1:
#ifdef CONFIG_RTW_PATH_DIV
			if (adapter->registrypriv.path_div == 1)
				txpath = txpath_1ss = HOST_BB_PATH_AUTO;
			else
#endif
				txpath = txpath_1ss = HOST_BB_PATH_A;
			break;
		default:
			goto exit;
		}
	} else
		txpath_1ss = txpath;

	if (hal_data->txpath_nss[0] != txpath_1ss) {
		hal_data->txpath_nss[0] = txpath_1ss;
		if (txpath_1ss == HOST_BB_PATH_AUTO)
			hal_data->txpath_num_nss[0] = 1;
		else {
			hal_data->txpath_num_nss[0] = 0;
			for (i = 0; i < RF_PATH_MAX; i++) {
				if (txpath_1ss & BIT(i))
					hal_data->txpath_num_nss[0]++;
			}
		}
	}
}
#endif

	hal_data->txpath = txpath;
	hal_data->rxpath = rxpath;
	dump_hal_runtime_trx_mode(NULL, adapter);
	rst = _SUCCESS;

exit:
	return rst;
}