/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * BMC multicast sta tx-rate helpers (extracted from core/rtw_ap.c, W3-80).
 *****************************************************************************/
#define _RTW_AP_BMC_UPDATE_C_

#include <drv_types.h>
#include <hal_data.h>

#if !defined(CONFIG_RUST_AP_BMC_UPDATE)

#ifdef CONFIG_BMC_TX_RATE_SELECT
u8 rtw_ap_find_bmc_rate(_adapter *adapter, u8 tx_rate);
u8 rtw_ap_find_mini_tx_rate(_adapter *adapter);

void rtw_update_bmc_sta_tx_rate(_adapter *adapter)
{
	struct sta_info *psta = NULL;
	u8 tx_rate;

	psta = rtw_get_bcmc_stainfo(adapter);
	if (psta == NULL) {
		RTW_ERR(ADPT_FMT "could not get bmc_sta !!\n", ADPT_ARG(adapter));
		return;
	}

	if (adapter->bmc_tx_rate != MGN_UNKNOWN) {
		psta->init_rate = adapter->bmc_tx_rate;
		goto _exit;
	}

	if (adapter->stapriv.asoc_sta_count <= 2)
		goto _exit;

	tx_rate = rtw_ap_find_mini_tx_rate(adapter);
	#ifdef CONFIG_BMC_TX_LOW_RATE
	tx_rate = rtw_ap_find_bmc_rate(adapter, tx_rate);
	#endif

	psta->init_rate = hw_rate_to_m_rate(tx_rate);

_exit:
	RTW_INFO(ADPT_FMT" BMC Tx rate - %s\n", ADPT_ARG(adapter), MGN_RATE_STR(psta->init_rate));
}
#endif /* CONFIG_BMC_TX_RATE_SELECT */

void rtw_init_bmc_sta_tx_rate(_adapter *padapter, struct sta_info *psta)
{
#ifdef CONFIG_BMC_TX_LOW_RATE
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
#endif
	u8 rate_idx = 0;
	u8 brate_table[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M,
		MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M};

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return;

	if (padapter->bmc_tx_rate != MGN_UNKNOWN)
		psta->init_rate = padapter->bmc_tx_rate;
	else {
		#ifdef CONFIG_BMC_TX_LOW_RATE
		if (IsEnableHWOFDM(pmlmeext->cur_wireless_mode) && (psta->cmn.ra_info.ramask && 0xFF0))
			rate_idx = get_lowest_rate_idx_ex(psta->cmn.ra_info.ramask, 4); /*from basic rate*/
		else
			rate_idx = get_lowest_rate_idx(psta->cmn.ra_info.ramask); /*from basic rate*/
		#else
		rate_idx = get_highest_rate_idx(psta->cmn.ra_info.ramask); /*from basic rate*/
		#endif
		if (rate_idx < 12)
			psta->init_rate = brate_table[rate_idx];
		else
			psta->init_rate = MGN_1M;
	}

	RTW_INFO(ADPT_FMT" BMC Init Tx rate - %s\n", ADPT_ARG(padapter), MGN_RATE_STR(psta->init_rate));
}

#endif /* !CONFIG_RUST_AP_BMC_UPDATE */

#if defined(CONFIG_RUST_AP_BMC_UPDATE)
void rtw_init_bmc_sta_tx_rate(_adapter *padapter, struct sta_info *psta);
void update_bmc_sta(_adapter *padapter);
#else

void update_bmc_sta(_adapter *padapter)
{
	_irqL	irqL;
	unsigned char	network_type;
	int supportRateNum = 0;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	WLAN_BSSID_EX *pcur_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;
	struct sta_info *psta = rtw_get_bcmc_stainfo(padapter);

	if (psta) {
		psta->cmn.aid = 0;/* default set to 0 */
#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(padapter))
			psta->qos_option = 1;
		else
#endif
			psta->qos_option = 0;
#ifdef CONFIG_80211N_HT
		psta->htpriv.ht_option = _FALSE;
#endif /* CONFIG_80211N_HT */

		psta->ieee8021x_blocked = 0;

		/* psta->dot118021XPrivacy = _NO_PRIVACY_; */ /* !!! remove it, because it has been set before this. */

		_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));

		supportRateNum = rtw_get_rateset_len((u8 *)&pcur_network->SupportedRates);
		network_type = rtw_check_network_type((u8 *)&pcur_network->SupportedRates, supportRateNum, pcur_network->Configuration.DSConfig);
		if (IsSupportedTxCCK(network_type))
			network_type = WIRELESS_11B;
		else if (network_type == WIRELESS_INVALID) { /* error handling */
			if (pcur_network->Configuration.DSConfig > 14)
				network_type = WIRELESS_11A;
			else
				network_type = WIRELESS_11B;
		}
		update_sta_basic_rate(psta, network_type);
		psta->wireless_mode = network_type;

		rtw_hal_update_sta_ra_info(padapter, psta);

		_enter_critical_bh(&psta->lock, &irqL);
		psta->state = WIFI_ASOC_STATE;
		_exit_critical_bh(&psta->lock, &irqL);

		rtw_sta_media_status_rpt(padapter, psta, 1);
		rtw_init_bmc_sta_tx_rate(padapter, psta);

	} else
		RTW_INFO("add_RATid_bmc_sta error!\n");
}

#endif /* !CONFIG_RUST_AP_BMC_UPDATE */
