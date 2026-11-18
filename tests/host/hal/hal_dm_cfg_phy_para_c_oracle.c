// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-110 — mirrors hal/hal_dm.c rtw_phydm_cfg_phy_para. */

#include "host_hal_dm_cfg_phy_para_types.h"

enum hal_status rtw_phydm_cfg_phy_para(struct dm_struct *p_dm_odm,
				       enum phydm_halmac_param config_type,
				       u32 offset, u32 data, u32 mask,
				       enum rf_path e_rf_path, u32 delay_time)
{
#ifdef RTW_HALMAC
	_adapter *adapter = p_dm_odm->adapter;
	struct rtw_phy_parameter para;

	switch (config_type) {
	case PHYDM_HALMAC_CMD_MAC_W8:
		para.cmd = 0;
		para.data.mac.offset = (uint16_t)offset;
		para.data.mac.value = data;
		para.data.mac.msk = mask;
		para.data.mac.msk_en = mask ? 1 : 0;
		para.data.mac.size = 1;
		break;
	case PHYDM_HALMAC_CMD_MAC_W16:
		para.cmd = 0;
		para.data.mac.offset = (uint16_t)offset;
		para.data.mac.value = data;
		para.data.mac.msk = mask;
		para.data.mac.msk_en = mask ? 1 : 0;
		para.data.mac.size = 2;
		break;
	case PHYDM_HALMAC_CMD_MAC_W32:
		para.cmd = 0;
		para.data.mac.offset = (uint16_t)offset;
		para.data.mac.value = data;
		para.data.mac.msk = mask;
		para.data.mac.msk_en = mask ? 1 : 0;
		para.data.mac.size = 4;
		break;
	case PHYDM_HALMAC_CMD_BB_W8:
		para.cmd = 1;
		para.data.bb.offset = (uint16_t)offset;
		para.data.bb.value = data;
		para.data.bb.msk = mask;
		para.data.bb.msk_en = mask ? 1 : 0;
		para.data.bb.size = 1;
		break;
	case PHYDM_HALMAC_CMD_BB_W16:
		para.cmd = 1;
		para.data.bb.offset = (uint16_t)offset;
		para.data.bb.value = data;
		para.data.bb.msk = mask;
		para.data.bb.msk_en = mask ? 1 : 0;
		para.data.bb.size = 2;
		break;
	case PHYDM_HALMAC_CMD_BB_W32:
		para.cmd = 1;
		para.data.bb.offset = (uint16_t)offset;
		para.data.bb.value = data;
		para.data.bb.msk = mask;
		para.data.bb.msk_en = mask ? 1 : 0;
		para.data.bb.size = 4;
		break;
	case PHYDM_HALMAC_CMD_RF_W:
		para.cmd = 2;
		para.data.rf.offset = (uint8_t)offset;
		para.data.rf.value = data;
		para.data.rf.msk = mask;
		para.data.rf.msk_en = mask ? 1 : 0;
		if (e_rf_path == RF_PATH_A)
			para.data.rf.path = 0;
		else if (e_rf_path == RF_PATH_B)
			para.data.rf.path = 1;
		else if (e_rf_path == RF_PATH_C)
			para.data.rf.path = 2;
		else if (e_rf_path == RF_PATH_D)
			para.data.rf.path = 3;
		else
			para.data.rf.path = 0;
		break;
	case PHYDM_HALMAC_CMD_DELAY_US:
		para.cmd = 3;
		para.data.delay.unit = 0;
		para.data.delay.value = (uint16_t)delay_time;
		break;
	case PHYDM_HALMAC_CMD_DELAY_MS:
		para.cmd = 3;
		para.data.delay.unit = 1;
		para.data.delay.value = (uint16_t)delay_time;
		break;
	case PHYDM_HALMAC_CMD_END:
		para.cmd = 0xFF;
		break;
	default:
		return HAL_STATUS_FAILURE;
	}

	if (rtw_halmac_cfg_phy_para(adapter_to_dvobj(adapter), &para))
		return HAL_STATUS_FAILURE;
#endif
	return HAL_STATUS_SUCCESS;
}