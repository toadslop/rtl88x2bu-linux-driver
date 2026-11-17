/* SPDX-License-Identifier: GPL-2.0 */
/* L1 reference leaf: HALMAC DM delegates (W4-109). */

typedef unsigned char u8;
typedef u8 boolean;

enum hal_status {
	HAL_STATUS_SUCCESS = 0,
	HAL_STATUS_FAILURE = 1,
};

typedef struct _adapter _adapter;
typedef struct dm_struct dm_struct;

boolean rtw_phydm_rfe_ctrl_gpio(_adapter *adapter, u8 gpio_num)
{
	(void)adapter;
	(void)gpio_num;
	return 0;
}

enum hal_status rtw_phydm_fw_iqk(struct dm_struct *p_dm_odm, u8 clear, u8 segment)
{
	(void)p_dm_odm;
	(void)clear;
	(void)segment;
	return HAL_STATUS_FAILURE;
}