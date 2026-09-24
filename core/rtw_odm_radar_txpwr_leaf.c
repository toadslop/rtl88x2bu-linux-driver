// SPDX-License-Identifier: GPL-2.0
#define _RTW_ODM_RADAR_TXPWR_LEAF_C_

#ifdef HOST_ODM_RADAR_TXPWR_TEST
#include "host_odm_radar_txpwr_types.h"
#else
#include <rtw_odm.h>
#include <hal_data.h>
#include <hal_com.h>
#endif

#if !defined(CONFIG_RUST) || defined(HOST_ODM_RADAR_TXPWR_TEST) || !defined(CONFIG_RUST_ODM_RADAR_TXPWR_LEAF)

s16 rtw_odm_get_tx_power_mbm(struct dm_struct *dm, u8 rfpath, u8 rate, u8 bw, u8 cch)
{
	return phy_get_txpwr_single_mbm(dm->adapter, rfpath, mgn_rate_to_rs(rate), rate, bw, cch, 0, 0, 0, NULL);
}

#ifdef CONFIG_DFS_MASTER
void rtw_odm_radar_detect_reset(_adapter *adapter)
{
	phydm_radar_detect_reset(adapter_to_phydm(adapter));
}

void rtw_odm_radar_detect_disable(_adapter *adapter)
{
	phydm_radar_detect_disable(adapter_to_phydm(adapter));
}

void rtw_odm_radar_detect_enable(_adapter *adapter)
{
	phydm_radar_detect_enable(adapter_to_phydm(adapter));
}

BOOLEAN rtw_odm_radar_detect(_adapter *adapter)
{
	return phydm_radar_detect(adapter_to_phydm(adapter));
}

static enum phydm_dfs_region_domain _rtw_dfs_regd_to_phydm[] = {
	[RTW_DFS_REGD_NONE]	= PHYDM_DFS_DOMAIN_UNKNOWN,
	[RTW_DFS_REGD_FCC]	= PHYDM_DFS_DOMAIN_FCC,
	[RTW_DFS_REGD_MKK]	= PHYDM_DFS_DOMAIN_MKK,
	[RTW_DFS_REGD_ETSI]	= PHYDM_DFS_DOMAIN_ETSI,
};

#define rtw_dfs_regd_to_phydm(region) \
	(((region) >= RTW_DFS_REGD_NUM) ? _rtw_dfs_regd_to_phydm[RTW_DFS_REGD_NONE] : \
	 _rtw_dfs_regd_to_phydm[(region)])

void rtw_odm_update_dfs_region(struct dvobj_priv *dvobj)
{
	odm_cmn_info_init(dvobj_to_phydm(dvobj), ODM_CMNINFO_DFS_REGION_DOMAIN,
			  rtw_dfs_regd_to_phydm(rtw_rfctl_get_dfs_domain(dvobj_to_rfctl(dvobj))));
}

u8 rtw_odm_radar_detect_polling_int_ms(struct dvobj_priv *dvobj)
{
	return phydm_dfs_polling_time(dvobj_to_phydm(dvobj));
}
#endif /* CONFIG_DFS_MASTER */

#endif
