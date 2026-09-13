// SPDX-License-Identifier: GPL-2.0
/*
 * Host oracle for BMC rate helpers:
 * - rtw_ap_find_bmc_rate: keep in sync with core/rtw_ap_rest.c
 * - rtw_ap_find_mini_tx_rate: keep in sync with core/rtw_ap.c until extract
 */
#include "host_ap_bmc_rate_types.h"

#define ODM_RATEVHTSS4MCS9 0x53

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
	u8 miini_tx_rate = ODM_RATEVHTSS4MCS9, sta_tx_rate;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &adapter->stapriv;

	_enter_critical_bh(&pstapriv->asoc_list_lock, &irqL);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		sta_tx_rate = psta->cmn.ra_info.curr_tx_rate & 0x7F;
		if (sta_tx_rate < miini_tx_rate)
			miini_tx_rate = sta_tx_rate;
	}
	_exit_critical_bh(&pstapriv->asoc_list_lock, &irqL);

	return miini_tx_rate;
}
