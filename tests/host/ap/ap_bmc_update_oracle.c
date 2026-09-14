// SPDX-License-Identifier: GPL-2.0
/* Host oracle — sync with core/rtw_ap_bmc_update.c + rtw_ap_rest.c helpers */
#include "host_ap_bmc_update_types.h"

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
#define ODM_RATEVHTSS4MCS9 0x53
#define MLME_IS_AP(a) (((a)->mlmepriv.state & WIFI_AP_STATE) != 0)
#define MLME_IS_MESH(a) 0

static u8 get_lowest_rate_idx_ex(u64 mask, int start_bit)
{
	int i;

	for (i = start_bit; i < 64; i++) {
		if ((mask >> i) & 0x01)
			return (u8)i;
	}
	return 0;
}

static u8 get_lowest_rate_idx(u64 mask)
{
	return get_lowest_rate_idx_ex(mask, 0);
}

#ifndef CONFIG_BMC_TX_LOW_RATE
static u8 get_highest_rate_idx(u64 mask)
{
	int i;

	for (i = 63; i >= 0; i--) {
		if ((mask >> i) & 0x01)
			return (u8)i;
	}
	return 0;
}
#endif

/* Legacy DESC rates 0x00–0x0b — same mapping as hal/hal_com.c */
static const u8 _hw_rate_to_m_rate[12] = {
	MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M, MGN_18M,
	MGN_24M, MGN_36M, MGN_48M, MGN_54M,
};

static u8 hw_rate_to_m_rate(u8 hw_rate)
{
	if (hw_rate < 12)
		return _hw_rate_to_m_rate[hw_rate];
	return MGN_1M;
}

struct sta_info *rtw_get_bcmc_stainfo(struct _adapter *padapter)
{
	return padapter->stapriv.host_bcmc_sta;
}

u8 rtw_ap_find_bmc_rate(struct _adapter *adapter, u8 tx_rate)
{
	u8 tx_ini_rate = 0x04;
	u8 band = GET_HAL_DATA(adapter)->current_band_type;

	switch (tx_rate) {
	case 0x49: case 0x48: case 0x47: case 0x46: case 0x45: case 0x44: case 0x43:
	case 0x3F: case 0x3E: case 0x3D: case 0x3C: case 0x3B: case 0x3A: case 0x39:
	case 0x35: case 0x34: case 0x33: case 0x32: case 0x31: case 0x30: case 0x2F:
	case 0x1B: case 0x1A: case 0x19: case 0x18: case 0x17: case 0x13: case 0x12:
	case 0x11: case 0x10: case 0x0F: case 0x0B: case 0x0A: case 0x09: case 0x08:
		tx_ini_rate = 0x08;
		break;
	case 0x42: case 0x41: case 0x38: case 0x37: case 0x2E: case 0x2D:
	case 0x16: case 0x15: case 0x0E: case 0x0D: case 0x07: case 0x06:
		tx_ini_rate = 0x06;
		break;
	case 0x40: case 0x36: case 0x2C: case 0x14: case 0x0C: case 0x05: case 0x04:
		tx_ini_rate = 0x04;
		break;
	case 0x03: case 0x02: case 0x01: case 0x00:
		tx_ini_rate = 0x00;
		break;
	default:
		tx_ini_rate = 0x04;
		break;
	}
	if (band == BAND_ON_5G && tx_ini_rate < 0x04)
		tx_ini_rate = 0x04;
	return tx_ini_rate;
}

u8 rtw_ap_find_mini_tx_rate(struct _adapter *adapter)
{
	_irqL irqL;
	struct _list *phead, *plist;
	u8 mini = ODM_RATEVHTSS4MCS9, sta_tx_rate;
	struct sta_info *psta;
	struct sta_priv *pstapriv = &adapter->stapriv;

	_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);
		sta_tx_rate = psta->cmn.ra_info.curr_tx_rate & 0x7F;
		if (sta_tx_rate < mini)
			mini = sta_tx_rate;
	}
	_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	return mini;
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
#ifdef CONFIG_BMC_TX_LOW_RATE
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
#endif
	u8 brate[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M, MGN_6M, MGN_9M, MGN_12M,
		      MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M};
	u8 rate_idx;

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return;
	if (padapter->bmc_tx_rate != MGN_UNKNOWN)
		psta->init_rate = padapter->bmc_tx_rate;
	else {
#ifdef CONFIG_BMC_TX_LOW_RATE
		if (IsEnableHWOFDM(pmlmeext->cur_wireless_mode) &&
		    (psta->cmn.ra_info.ramask && 0xFF0))
			rate_idx = get_lowest_rate_idx_ex(psta->cmn.ra_info.ramask, 4);
		else
			rate_idx = get_lowest_rate_idx(psta->cmn.ra_info.ramask);
#else
		rate_idx = get_highest_rate_idx(psta->cmn.ra_info.ramask);
#endif
		psta->init_rate = (rate_idx < 12) ? brate[rate_idx] : MGN_1M;
	}
}
