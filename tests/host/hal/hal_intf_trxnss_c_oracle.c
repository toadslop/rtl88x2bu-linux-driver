// SPDX-License-Identifier: GPL-2.0
/* Host L2 C oracle for rtw_hal_trxnss_init (W4-66). */

#include <stddef.h>

#include "host_hal_intf_trxnss_types.h"

static const u8 _rf_type_to_rf_rx_cnt[16] = {
	1, 2, 2, 3, 4, 3, 4, 4, 3, 2, 1, 2, 1, 1, 4, 3,
};

static u8 rf_type_to_rf_rx_cnt(u8 rf_type)
{
	if (rf_type >= 16)
		return 0;
	return _rf_type_to_rf_rx_cnt[rf_type];
}

static u8 rtw_min_u8(u8 a, u8 b)
{
	return a > b ? b : a;
}

void host_hal_trxnss_init_common(const struct host_hal_trxnss_init_in *in,
				 struct host_hal_trxnss_init_out *out)
{
	u8 tx_nss;
	u8 rx_nss;
	int i;

	if (!in || !out)
		return;

	tx_nss = (u8)in->hal_spec_tx_nss;
	rx_nss = (u8)in->hal_spec_rx_nss;

	if (in->reg_tx_nss > 0)
		tx_nss = rtw_min_u8(tx_nss, (u8)in->reg_tx_nss);
	tx_nss = rtw_min_u8(tx_nss, in->max_tx_cnt);
	if (in->reg_rx_nss > 0)
		rx_nss = rtw_min_u8(rx_nss, (u8)in->reg_rx_nss);
	rx_nss = rtw_min_u8(rx_nss, rf_type_to_rf_rx_cnt(in->rf_path));

	for (i = 0; i < 4; i++)
		out->txpath_cap_num_nss[i] = 0;

	for (i = 0; i < 4; i++) {
		if (tx_nss < (u8)(i + 1))
			break;
		if (in->hw_full_tx)
			out->txpath_cap_num_nss[i] = in->max_tx_cnt;
		else
			out->txpath_cap_num_nss[i] = (u8)(i + 1);
	}

	out->tx_nss = tx_nss;
	out->rx_nss = rx_nss;
}
