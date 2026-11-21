// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_tx_aclt_set_types.h"
#include <string.h>

static struct _adapter g_adapter;

void host_tx_aclt_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.hal.hal_spec.tx_aclt_unit_factor = 1;
}

struct _adapter *host_tx_aclt_adapter(void)
{
	return &g_adapter;
}

#ifndef RUST_HAL_COM_TX_ACLT_SET_ORACLE
void rtw_hal_set_tx_aclt_force_val_c_oracle(struct _adapter *, struct tx_aclt_conf_t *, u8);
void rtw_hal_set_tx_aclt_conf_c_oracle(struct _adapter *, u8, struct tx_aclt_conf_t *, u8);

void rtw_hal_set_tx_aclt_force_val(struct _adapter *a, struct tx_aclt_conf_t *in, u8 n)
{
	rtw_hal_set_tx_aclt_force_val_c_oracle(a, in, n);
}

void rtw_hal_set_tx_aclt_conf(struct _adapter *a, u8 idx, struct tx_aclt_conf_t *in, u8 n)
{
	rtw_hal_set_tx_aclt_conf_c_oracle(a, idx, in, n);
}
#endif