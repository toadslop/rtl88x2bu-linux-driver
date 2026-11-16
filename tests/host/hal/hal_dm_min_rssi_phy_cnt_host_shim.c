#include "host_hal_dm_min_rssi_phy_cnt_types.h"

struct macid_ctl_t *rtw_rust_hal_dm_macidctl(_adapter *adapter)
{
	return adapter_to_macidctl(adapter);
}

struct sta_info *rtw_rust_hal_dm_macid_sta(struct macid_ctl_t *ctl, int i)
{
	if (!ctl || i < 0 || i >= MACID_NUM_SW_LIMIT)
		return NULL;
	return ctl->sta[i];
}

u8 rtw_rust_hal_dm_macid_h2c_opmode(struct macid_ctl_t *ctl, int i)
{
	if (!ctl || i < 0 || i >= MACID_NUM_SW_LIMIT)
		return 0;
	return GET_H2CCMD_MSRRPT_PARM_OPMODE(ctl->h2c_msr + i);
}

s8 rtw_rust_hal_dm_sta_rssi(struct sta_info *sta)
{
	if (!sta)
		return 0;
	return sta->cmn.rssi_stat.rssi;
}

u8 rtw_rust_hal_dm_is_broadcast_sta(struct sta_info *sta)
{
	if (!sta)
		return 1;
	return is_broadcast_mac_addr(sta->cmn.mac_addr) ? 1 : 0;
}

struct dm_struct *rtw_rust_hal_dm_adapter_to_phydm(_adapter *adapter)
{
	return adapter_to_phydm(adapter);
}