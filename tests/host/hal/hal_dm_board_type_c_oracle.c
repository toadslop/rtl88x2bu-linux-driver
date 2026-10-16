// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-30 Init_ODM_ComInfo board-type block (hal/hal_dm.c). */

#include "host_hal_dm_board_type_types.h"

void rtw_rust_hal_dm_init_odm_board_type(
	struct dm_struct *pdm_odm,
	u8 external_lna_2g,
	u8 external_lna_5g,
	u8 external_pa_2g,
	u8 external_pa_5g,
	u8 eeprom_bt_coexist)
{
	u8 odm_board_type = ODM_BOARD_DEFAULT;

	if (!pdm_odm)
		return;

	if (external_lna_2g != 0) {
		odm_board_type |= ODM_BOARD_EXT_LNA;
		odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_LNA, 1);
	}
	if (external_lna_5g != 0) {
		odm_board_type |= ODM_BOARD_EXT_LNA_5G;
		odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_LNA, 1);
	}
	if (external_pa_2g != 0) {
		odm_board_type |= ODM_BOARD_EXT_PA;
		odm_cmn_info_init(pdm_odm, ODM_CMNINFO_EXT_PA, 1);
	}
	if (external_pa_5g != 0) {
		odm_board_type |= ODM_BOARD_EXT_PA_5G;
		odm_cmn_info_init(pdm_odm, ODM_CMNINFO_5G_EXT_PA, 1);
	}
	if (eeprom_bt_coexist)
		odm_board_type |= ODM_BOARD_BT;

	odm_cmn_info_init(pdm_odm, ODM_CMNINFO_BOARD_TYPE, odm_board_type);
}
