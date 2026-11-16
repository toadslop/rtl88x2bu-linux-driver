// SPDX-License-Identifier: GPL-2.0
#include "host_hal_dm_set_odm_var_rx_types.h"

extern void host_hal_dm_apply_rx_fixture_to_dm(_adapter *a);

void SetHalODMVar(
	_adapter *Adapter,
	u32 eVariable,
	void *pValue1,
	BOOLEAN bSet)
{
	struct dm_struct *podmpriv = adapter_to_phydm(Adapter);

	(void)bSet;
	host_hal_dm_apply_rx_fixture_to_dm(Adapter);

	switch (eVariable) {
	case HAL_ODM_RX_INFO_DUMP: {
		u8 cur_igi = 0;
		s8 rssi_min;
		void *sel;

		sel = pValue1;
		cur_igi = rtw_phydm_get_cur_igi(Adapter);
		rssi_min = rtw_phydm_get_min_rssi(Adapter);

		_RTW_PRINT_SEL(sel, "============ Rx Info dump ===================\n");
		_RTW_PRINT_SEL(sel,
			       "is_linked = %d, rssi_min = %d(%%)(%d(%%)), current_igi = 0x%x\n",
			       podmpriv->is_linked, rssi_min, rtw_dm_get_min_rssi(Adapter),
			       cur_igi);
		_RTW_PRINT_SEL(sel,
			       "cnt_cck_fail = %d, cnt_ofdm_fail = %d, Total False Alarm = %d\n",
			       rtw_phydm_get_phy_cnt(Adapter, FA_CCK),
			       rtw_phydm_get_phy_cnt(Adapter, FA_OFDM),
			       rtw_phydm_get_phy_cnt(Adapter, FA_TOTAL));

		if (podmpriv->is_linked) {
			_RTW_PRINT_SEL(sel, "rx_rate = %s",
				       host_hdata_rate_str(podmpriv->rx_rate));
			if (host_is_hardware_type_8814a(Adapter))
				_RTW_PRINT_SEL(sel,
					       " rssi_a = %d(%%), rssi_b = %d(%%), rssi_c = %d(%%), rssi_d = %d(%%)\n",
					       podmpriv->rssi_a, podmpriv->rssi_b,
					       podmpriv->rssi_c, podmpriv->rssi_d);
			else
				_RTW_PRINT_SEL(sel, " rssi_a = %d(%%), rssi_b = %d(%%)\n",
					       podmpriv->rssi_a, podmpriv->rssi_b);
		}
	}
	break;
	case HAL_ODM_RX_DFRAME_INFO:
		(void)pValue1;
		break;
	default:
		break;
	}
}