// SPDX-License-Identifier: GPL-2.0
/* Host oracle — sync with core/rtw_ap_bmc_update.c */
#include "host_ap_bmc_rate_types.h"

#define MGN_UNKNOWN 0x00
#define MGN_1M 0x02
#define MGN_2M 0x04
#define MGN_5_5M 0x0B
#define MGN_6M 0x0C
#define MGN_9M 0x12
#define MGN_11M 0x16
#define MGN_12M 0x18
#define MGN_18M 0x24
#define MGN_24M 0x30
#define MGN_36M 0x48
#define MGN_48M 0x60
#define MGN_54M 0x6C
#define MLME_IS_AP(a) (((a)->mlmepriv.state & WIFI_AP_STATE) != 0)
#define MLME_IS_MESH(a) 0

static u8 get_lowest_rate_idx(u64 mask)
{
	int i;

	for (i = 0; i < 64; i++) {
		if ((mask >> i) & 1)
			return (u8)i;
	}
	return 0;
}

static u8 hw_rate_to_m_rate(u8 hw_rate)
{
	if (hw_rate == 0x04)
		return MGN_6M;
	if (hw_rate == 0x08)
		return MGN_24M;
	return MGN_1M;
}

struct sta_info *rtw_get_bcmc_stainfo(struct _adapter *padapter)
{
	return padapter->stapriv.host_bcmc_sta;
}

#ifdef CONFIG_BMC_TX_RATE_SELECT
void rtw_update_bmc_sta_tx_rate(struct _adapter *adapter)
{
	struct sta_info *psta;
	u8 tx_rate;

	psta = rtw_get_bcmc_stainfo(adapter);
	if (!psta)
		return;
	if (adapter->bmc_tx_rate != MGN_UNKNOWN) {
		psta->init_rate = adapter->bmc_tx_rate;
		return;
	}
	if (adapter->stapriv.asoc_sta_count <= 2)
		return;
	tx_rate = rtw_ap_find_mini_tx_rate(adapter);
#ifdef CONFIG_BMC_TX_LOW_RATE
	tx_rate = rtw_ap_find_bmc_rate(adapter, tx_rate);
#endif
	psta->init_rate = hw_rate_to_m_rate(tx_rate);
}
#endif

void rtw_init_bmc_sta_tx_rate(struct _adapter *padapter, struct sta_info *psta)
{
	u8 brate[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M,
		      MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M};
	u8 rate_idx;

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return;
	if (padapter->bmc_tx_rate != MGN_UNKNOWN)
		psta->init_rate = padapter->bmc_tx_rate;
	else {
		rate_idx = get_lowest_rate_idx(psta->cmn.ra_info.ramask);
		psta->init_rate = (rate_idx < 12) ? brate[rate_idx] : MGN_1M;
	}
}
