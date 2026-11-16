// SPDX-License-Identifier: GPL-2.0
/* C oracle + host NSS shims for W4-97. */

#include "host_hal_update_sta_types.h"

static s8 host_sta_tx_nss = 1;
static s8 host_sta_rx_nss = 1;
static int host_warn_on_called;

void host_hal_update_sta_reset_nss(void)
{
	host_sta_tx_nss = 1;
	host_sta_rx_nss = 1;
	host_warn_on_called = 0;
}

void host_hal_update_sta_set_nss(s8 tx_nss, s8 rx_nss)
{
	host_sta_tx_nss = tx_nss;
	host_sta_rx_nss = rx_nss;
}

int host_hal_update_sta_warn_on_called(void) { return host_warn_on_called; }

s8 rtw_get_sta_tx_nss(struct _adapter *a, struct host_sta_info *p)
{
	(void)a;
	(void)p;
	return host_sta_tx_nss;
}

s8 rtw_get_sta_rx_nss(struct _adapter *a, struct host_sta_info *p)
{
	(void)a;
	(void)p;
	return host_sta_rx_nss;
}

void rtw_warn_on(int c)
{
	if (c)
		host_warn_on_called = 1;
}

void rtw_hal_update_sta_wset(struct _adapter *adapter, struct host_sta_info *psta)
{
	u8 w_set = 0;

	(void)adapter;
	if (psta->wireless_mode & WIRELESS_11B)
		w_set |= WIRELESS_CCK;
	if ((psta->wireless_mode & WIRELESS_11G) ||
	    (psta->wireless_mode & WIRELESS_11A))
		w_set |= WIRELESS_OFDM;
	if ((psta->wireless_mode & WIRELESS_11_24N) ||
	    (psta->wireless_mode & WIRELESS_11_5N))
		w_set |= WIRELESS_HT;
	if (psta->wireless_mode & WIRELESS_11AC)
		w_set |= WIRELESS_VHT;
	psta->cmn.support_wireless_set = w_set;
}

void rtw_hal_update_sta_mimo_type(struct _adapter *adapter, struct host_sta_info *psta)
{
	s8 tx_nss = rtw_get_sta_tx_nss(adapter, psta);
	s8 rx_nss = rtw_get_sta_rx_nss(adapter, psta);

	(void)adapter;
	if (tx_nss == 1 && rx_nss == 1)
		psta->cmn.mimo_type = RF_1T1R;
	else if (tx_nss == 1 && rx_nss == 2)
		psta->cmn.mimo_type = RF_1T2R;
	else if (tx_nss == 2 && rx_nss == 2)
		psta->cmn.mimo_type = RF_2T2R;
	else if (tx_nss == 2 && rx_nss == 3)
		psta->cmn.mimo_type = RF_2T3R;
	else if (tx_nss == 2 && rx_nss == 4)
		psta->cmn.mimo_type = RF_2T4R;
	else if (tx_nss == 3 && rx_nss == 3)
		psta->cmn.mimo_type = RF_3T3R;
	else if (tx_nss == 3 && rx_nss == 4)
		psta->cmn.mimo_type = RF_3T4R;
	else if (tx_nss == 4 && rx_nss == 4)
		psta->cmn.mimo_type = RF_4T4R;
	else
		rtw_warn_on(1);
}