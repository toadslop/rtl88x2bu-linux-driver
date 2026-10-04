// SPDX-License-Identifier: GPL-2.0
#define _RTW_ODM_ADAPTIVITY_LEAF_C_

#ifdef HOST_ODM_ADAPTIVITY_TEST
#include "host_odm_adaptivity_types.h"
#else
#include <rtw_odm.h>
#include <hal_data.h>
#include <hal_com.h>
#endif

#define RTW_ADAPTIVITY_EN_DISABLE 0
#define RTW_ADAPTIVITY_EN_ENABLE 1
#define RTW_ADAPTIVITY_MODE_NORMAL 0
#define RTW_ADAPTIVITY_MODE_CARRIER_SENSE 1

#if !defined(CONFIG_RUST) || defined(HOST_ODM_ADAPTIVITY_TEST) || !defined(CONFIG_RUST_ODM_ADAPTIVITY_LEAF)

void rtw_odm_adaptivity_ver_msg(void *sel, _adapter *adapter)
{
	(void)adapter;
	RTW_PRINT_SEL(sel, "ADAPTIVITY_VERSION " ADAPTIVITY_VERSION "\n");
}

void rtw_odm_adaptivity_en_msg(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;

	RTW_PRINT_SEL(sel, "RTW_ADAPTIVITY_EN_");

	if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_DISABLE)
		_RTW_PRINT_SEL(sel, "DISABLE\n");
	else if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_ENABLE)
		_RTW_PRINT_SEL(sel, "ENABLE\n");
	else
		_RTW_PRINT_SEL(sel, "INVALID\n");
}

void rtw_odm_adaptivity_mode_msg(void *sel, _adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;

	RTW_PRINT_SEL(sel, "RTW_ADAPTIVITY_MODE_");

	if (regsty->adaptivity_mode == RTW_ADAPTIVITY_MODE_NORMAL)
		_RTW_PRINT_SEL(sel, "NORMAL\n");
	else if (regsty->adaptivity_mode == RTW_ADAPTIVITY_MODE_CARRIER_SENSE)
		_RTW_PRINT_SEL(sel, "CARRIER_SENSE\n");
	else
		_RTW_PRINT_SEL(sel, "INVALID\n");
}

void rtw_odm_adaptivity_config_msg(void *sel, _adapter *adapter)
{
	rtw_odm_adaptivity_ver_msg(sel, adapter);
	rtw_odm_adaptivity_en_msg(sel, adapter);
	rtw_odm_adaptivity_mode_msg(sel, adapter);
}

bool rtw_odm_adaptivity_needed(_adapter *adapter)
{
	struct registry_priv *regsty = &adapter->registrypriv;
	bool ret = _FALSE;

	if (regsty->adaptivity_en == RTW_ADAPTIVITY_EN_ENABLE)
		ret = _TRUE;

	return ret;
}

void rtw_odm_adaptivity_parm_msg(void *sel, _adapter *adapter)
{
	struct dm_struct *odm = adapter_to_phydm(adapter);

	rtw_odm_adaptivity_config_msg(sel, adapter);

	RTW_PRINT_SEL(sel, "%10s %16s\n", "th_l2h_ini", "th_edcca_hl_diff");
	RTW_PRINT_SEL(sel, "0x%-8x %-16d\n", (u8)odm->th_l2h_ini, odm->th_edcca_hl_diff);
}

void rtw_odm_adaptivity_parm_set(_adapter *adapter, s8 th_l2h_ini, s8 th_edcca_hl_diff)
{
	struct dm_struct *odm = adapter_to_phydm(adapter);

	odm->th_l2h_ini = th_l2h_ini;
	odm->th_edcca_hl_diff = th_edcca_hl_diff;
}

void rtw_odm_get_perpkt_rssi(void *sel, _adapter *adapter)
{
	struct dm_struct *odm = adapter_to_phydm(adapter);

	RTW_PRINT_SEL(sel, "rx_rate = %s, rssi_a = %d(%%), rssi_b = %d(%%)\n",
		      HDATA_RATE(odm->rx_rate), odm->rssi_a, odm->rssi_b);
}

#endif
