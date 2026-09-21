// SPDX-License-Identifier: GPL-2.0
#include "host_tdls_vht_types.h"

static u8 g_hal_tx_nss = 2;

u8 host_tdls_hal_tx_nss(_adapter *a)
{
	(void)a;
	return g_hal_tx_nss;
}

u8 host_tdls_hal_bw_support(_adapter *a, u8 bw)
{
	(void)a;
	return bw <= CHANNEL_WIDTH_80 ? _TRUE : _FALSE;
}

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map)
{
	u8 i, j, cur_rate, target_rate;

	for (i = 0; i < 2; i++) {
		target_mcs_map[i] = 0;
		for (j = 0; j < 8; j += 2) {
			cur_rate = (cur_mcs_map[i] >> j) & 3;
			target_rate = (cur_rate == 3 || nss <= ((j / 2) + i * 4)) ? 3 : cur_rate;
			target_mcs_map[i] |= (target_rate << j);
		}
	}
}
