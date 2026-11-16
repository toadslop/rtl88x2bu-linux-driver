// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-93 min RSSI and PHY counter queries (hal/hal_dm.c). */

#include "host_hal_dm_min_rssi_phy_cnt_types.h"

s8 rtw_dm_get_min_rssi(_adapter *adapter)
{
	struct macid_ctl_t *macid_ctl = adapter_to_macidctl(adapter);
	struct sta_info *sta;
	s8 min_rssi = 127, rssi;
	int i;

	for (i = 0; i < MACID_NUM_SW_LIMIT; i++) {
		sta = macid_ctl->sta[i];
		if (!sta || !GET_H2CCMD_MSRRPT_PARM_OPMODE(macid_ctl->h2c_msr + i)
		    || is_broadcast_mac_addr(sta->cmn.mac_addr))
			continue;
		rssi = sta->cmn.rssi_stat.rssi;
		if (rssi >= 0 && min_rssi > rssi)
			min_rssi = rssi;
	}

	return min_rssi == 127 ? 0 : min_rssi;
}

s8 rtw_phydm_get_min_rssi(_adapter *adapter)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	s8 rssi_min = 0;

	rssi_min = (s8)phydm_cmn_info_query(phydm, PHYDM_INFO_RSSI_MIN);
	return rssi_min;
}

boolean rtw_phydm_get_edcca_flag(_adapter *adapter)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	boolean cur_edcca_flag = 0;

	cur_edcca_flag = (boolean)phydm_cmn_info_query(phydm, PHYDM_INFO_EDCCA_FLAG);
	return cur_edcca_flag;
}

u32 rtw_phydm_get_phy_cnt(_adapter *adapter, enum phy_cnt cnt)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);

	if (cnt == FA_OFDM)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_FA_OFDM);
	else if (cnt == FA_CCK)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_FA_CCK);
	else if (cnt == FA_TOTAL)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_FA_TOTAL);
	else if (cnt == CCA_OFDM)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CCA_OFDM);
	else if (cnt == CCA_CCK)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CCA_CCK);
	else if (cnt == CCA_ALL)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CCA_ALL);
	else if (cnt == CRC32_OK_VHT)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_OK_VHT);
	else if (cnt == CRC32_OK_HT)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_OK_HT);
	else if (cnt == CRC32_OK_LEGACY)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_OK_LEGACY);
	else if (cnt == CRC32_OK_CCK)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_OK_CCK);
	else if (cnt == CRC32_ERROR_VHT)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_ERROR_VHT);
	else if (cnt == CRC32_ERROR_HT)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_ERROR_HT);
	else if (cnt == CRC32_ERROR_LEGACY)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_ERROR_LEGACY);
	else if (cnt == CRC32_ERROR_CCK)
		return phydm_cmn_info_query(phydm, PHYDM_INFO_CRC32_ERROR_CCK);
	else
		return 0;
}