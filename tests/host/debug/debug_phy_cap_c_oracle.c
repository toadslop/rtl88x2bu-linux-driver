// SPDX-License-Identifier: GPL-2.0
/* C oracle for W3-210 PHY cap dump helpers (from core/rtw_debug_rest.c). */

#include "host_rtw_debug_phy_cap_types.h"

#ifndef PHY_CAP_C_REF
void rtw_ht_use_default_setting(_adapter *padapter)
{
	(void)padapter;
}

void rtw_vht_use_default_setting(_adapter *padapter)
{
	(void)padapter;
}
#endif

void rtw_dump_dft_phy_cap(void *sel, _adapter *adapter)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
#ifdef CONFIG_80211AC_VHT
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;
#endif

#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT STBC Tx : %s\n",
		      (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT STBC Rx : %s\n",
		      (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX)) ? "V" : "X");
#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT STBC Tx : %s\n",
		      (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT STBC Rx : %s\n\n",
		      (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_RX)) ? "V" : "X");

#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT LDPC Tx : %s\n",
		      (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT LDPC Rx : %s\n",
		      (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX)) ? "V" : "X");
#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT LDPC Tx : %s\n",
		      (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_TX)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT LDPC Rx : %s\n\n",
		      (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_RX)) ? "V" : "X");

#ifdef CONFIG_BEAMFORMING
#ifdef CONFIG_80211AC_VHT
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT MU Bfer : %s\n",
		      (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT MU Bfee : %s\n",
		      (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT SU Bfer : %s\n",
		      (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] VHT SU Bfee : %s\n",
		      (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)) ? "V" : "X");
#endif
	RTW_PRINT_SEL(sel, "[DFT CAP] HT Bfer : %s\n",
		      (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DFT CAP] HT Bfee : %s\n",
		      (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE)) ? "V" : "X");
#endif
}

void rtw_get_dft_phy_cap(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "\n ======== PHY CAP protocol ========\n");
#ifndef PHY_CAP_C_REF
	rtw_ht_use_default_setting(adapter);
#ifdef CONFIG_80211AC_VHT
	rtw_vht_use_default_setting(adapter);
#endif
#endif
	rtw_dump_dft_phy_cap(sel, adapter);
}

void rtw_dump_drv_phy_cap(void *sel, _adapter *adapter)
{
	struct registry_priv *pregistry_priv = &adapter->registrypriv;

	RTW_PRINT_SEL(sel, "\n ======== DRV's configuration ========\n");
	RTW_PRINT_SEL(sel, "[DRV CAP] STBC Capability : 0x%02x\n", pregistry_priv->stbc_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT STBC Tx : %s\n",
		      (TEST_FLAG(pregistry_priv->stbc_cap, BIT1)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT STBC Rx : %s\n",
		      (TEST_FLAG(pregistry_priv->stbc_cap, BIT0)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT STBC Tx : %s\n",
		      (TEST_FLAG(pregistry_priv->stbc_cap, BIT5)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT STBC Rx : %s\n\n",
		      (TEST_FLAG(pregistry_priv->stbc_cap, BIT4)) ? "V" : "X");

	RTW_PRINT_SEL(sel, "[DRV CAP] LDPC Capability : 0x%02x\n", pregistry_priv->ldpc_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT LDPC Tx : %s\n",
		      (TEST_FLAG(pregistry_priv->ldpc_cap, BIT1)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT LDPC Rx : %s\n",
		      (TEST_FLAG(pregistry_priv->ldpc_cap, BIT0)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT LDPC Tx : %s\n",
		      (TEST_FLAG(pregistry_priv->ldpc_cap, BIT5)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT LDPC Rx : %s\n\n",
		      (TEST_FLAG(pregistry_priv->ldpc_cap, BIT4)) ? "V" : "X");

#ifdef CONFIG_BEAMFORMING
	RTW_PRINT_SEL(sel, "[DRV CAP] TxBF Capability : 0x%02x\n", pregistry_priv->beamform_cap);
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT MU Bfer : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT2)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT MU Bfee : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT3)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT SU Bfer : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT0)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] VHT SU Bfee : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT1)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Bfer : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT4)) ? "V" : "X");
	RTW_PRINT_SEL(sel, "[DRV CAP] HT Bfee : %s\n",
		      (TEST_FLAG(pregistry_priv->beamform_cap, BIT5)) ? "V" : "X");

	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Bfer rf_num : %d\n", pregistry_priv->beamformer_rf_num);
	RTW_PRINT_SEL(sel, "[DRV CAP] Tx Bfee rf_num : %d\n", pregistry_priv->beamformee_rf_num);
#endif
}
