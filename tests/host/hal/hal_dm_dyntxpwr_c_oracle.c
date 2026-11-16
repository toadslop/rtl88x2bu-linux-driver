// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-100 — mirrors hal/hal_dm.c dynamic TX power desc helpers. */

#include "host_hal_dm_dyntxpwr_types.h"

#ifdef CONFIG_SUPPORT_DYNAMIC_TXPWR
void rtw_phydm_fill_desc_dpt(void *dm, u8 *desc, u8 dpt_lv)
{
	struct host_dm_dyntxpwr *p_dm = (struct host_dm_dyntxpwr *)dm;
	host_dyntxpwr_adapter *adapter;

	if (!p_dm)
		return;
	adapter = p_dm->adapter;
	if (!adapter)
		return;

	switch (host_get_chip_type(adapter)) {
#ifdef CONFIG_RTL8822B
	case HOST_RTL8822B:
		host_set_tx_desc_txpwr_ofset_8822b(desc, dpt_lv);
		break;
#endif
	default:
		host_hal_dm_dyntxpwr_log_unsupported();
		break;
	}
}

void rtw_phydm_set_dyntxpwr(host_dyntxpwr_adapter *adapter, u8 *desc, u8 mac_id)
{
	struct host_dm_dyntxpwr *dm = adapter_to_phydm_dyntxpwr(adapter);

	if (!dm)
		return;
	odm_set_dyntxpwr(dm, desc, mac_id);
}
#endif /* CONFIG_SUPPORT_DYNAMIC_TXPWR */