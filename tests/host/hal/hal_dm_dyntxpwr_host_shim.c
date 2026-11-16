// SPDX-License-Identifier: GPL-2.0
/* Host shims for W4-100 dynamic TX power desc helpers L2. */

#include <string.h>
#include "host_hal_dm_dyntxpwr_types.h"

struct host_hal_dm_dyntxpwr_trace host_hal_dm_dyntxpwr_trace;

void host_hal_dm_dyntxpwr_reset(void)
{
	memset(&host_hal_dm_dyntxpwr_trace, 0,
	       sizeof(host_hal_dm_dyntxpwr_trace));
}

void host_hal_dm_dyntxpwr_log_unsupported(void)
{
	host_hal_dm_dyntxpwr_trace.err_logged = 1;
}

u8 host_get_chip_type(host_dyntxpwr_adapter *adapter)
{
	if (!adapter)
		return HOST_NULL_CHIP_TYPE;
	return adapter->dvobj.chip_type;
}

struct host_dm_dyntxpwr *adapter_to_phydm_dyntxpwr(host_dyntxpwr_adapter *adapter)
{
	if (!adapter)
		return NULL;
	adapter->odmpriv.adapter = adapter;
	return &adapter->odmpriv;
}

void host_set_tx_desc_txpwr_ofset_8822b(u8 *txdesc, u8 value)
{
	u32 *dword5;

	if (!txdesc)
		return;
	dword5 = (u32 *)(txdesc + HOST_HALMAC_TX_DESC_DW5_OFF);
	*dword5 = (*dword5 & ~(0x7u << 28)) | ((value & 0x7u) << 28);
}

u8 host_get_tx_desc_txpwr_ofset_8822b(const u8 *txdesc)
{
	u32 dword5;

	if (!txdesc)
		return 0;
	dword5 = *((const u32 *)(txdesc + HOST_HALMAC_TX_DESC_DW5_OFF));
	return (u8)((dword5 >> 28) & 0x7u);
}

static int host_is_sta_active(struct host_cmn_sta_dyntxpwr *sta)
{
	return sta && (sta->dm_ctrl & HOST_STA_DM_CTRL_ACTIVE);
}

void odm_set_dyntxpwr(void *dm_void, u8 *desc, u8 sta_id)
{
	struct host_dm_dyntxpwr *dm = (struct host_dm_dyntxpwr *)dm_void;
	struct host_cmn_sta_dyntxpwr *sta;
	struct host_dtp_info *dtp;

	host_hal_dm_dyntxpwr_trace.odm_set_calls++;
	host_hal_dm_dyntxpwr_trace.last_mac_id = sta_id;

	if (!dm || sta_id >= HOST_DYNTPWR_MAX_STA)
		return;
	sta = dm->phydm_sta_info[sta_id];
	if (!host_is_sta_active(sta))
		return;
	dtp = &sta->dtp_stat;

	if (!(dm->support_ability & HOST_ODM_BB_DYNAMIC_TXPWR))
		return;

	if (dm->fill_desc_dyntxpwr)
		dm->fill_desc_dyntxpwr(dm, desc, dtp->dyn_tx_power);

	if (dtp->last_tx_power != dtp->dyn_tx_power)
		dtp->last_tx_power = dtp->dyn_tx_power;
}

host_dyntxpwr_adapter *rtw_rust_hal_dm_phydm_adapter(struct host_dm_dyntxpwr *dm)
{
	return dm ? dm->adapter : NULL;
}

u8 rtw_rust_hal_dm_get_chip_type(host_dyntxpwr_adapter *adapter)
{
	return host_get_chip_type(adapter);
}

struct host_dm_dyntxpwr *rtw_rust_hal_dm_adapter_to_phydm(host_dyntxpwr_adapter *adapter)
{
	return adapter_to_phydm_dyntxpwr(adapter);
}

void rtw_rust_hal_dm_set_tx_desc_txpwr_ofset_8822b(u8 *desc, u8 value)
{
	host_set_tx_desc_txpwr_ofset_8822b(desc, value);
}

void rtw_rust_hal_dm_log_dyntxpwr_unsupported(void)
{
	host_hal_dm_dyntxpwr_log_unsupported();
}

void rtw_rust_hal_dm_odm_set_dyntxpwr(void *dm, u8 *desc, u8 mac_id)
{
	odm_set_dyntxpwr(dm, desc, mac_id);
}
