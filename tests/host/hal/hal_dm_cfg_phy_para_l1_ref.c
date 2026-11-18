// SPDX-License-Identifier: GPL-2.0
/* L1 reference leaf: PHYDM HALMAC cfg_phy_para mapper (W4-110). */

typedef void dm_struct;

enum hal_status {
	HAL_STATUS_SUCCESS = 0,
	HAL_STATUS_FAILURE = 1,
};

enum hal_status rtw_phydm_cfg_phy_para(dm_struct *p_dm_odm, int config_type,
				       unsigned int offset, unsigned int data,
				       unsigned int mask, int e_rf_path,
				       unsigned int delay_time)
{
	(void)p_dm_odm;
	(void)config_type;
	(void)offset;
	(void)data;
	(void)mask;
	(void)e_rf_path;
	(void)delay_time;
	return HAL_STATUS_SUCCESS;
}