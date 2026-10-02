// SPDX-License-Identifier: GPL-2.0
#include "host_tdls_vht_types.h"

static u8 g_hal_tx_nss = 2;

void host_tdls_set_hal_tx_nss(u8 nss)
{
	g_hal_tx_nss = nss;
}

u8 host_tdls_hal_tx_nss(_adapter *a)
{
	(void)a;
	return g_hal_tx_nss;
}
