// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_tx_aclt_set_types.h"
#include <string.h>

static struct _adapter g_adapter;

void host_tx_aclt_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	g_adapter.hal.hal_spec.tx_aclt_unit_factor = 1;
}

static struct _adapter *to_adap(void *p)
{
	return (struct _adapter *)p;
}

u8 rtw_rust_hal_com_tx_aclt_unit_factor(void *p)
{
	return GET_HAL_SPEC(to_adap(p))->tx_aclt_unit_factor;
}

struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_force_conf(void *p)
{
	return &adapter_to_dvobj(to_adap(p))->tx_aclt_force_val;
}

struct tx_aclt_conf_t *rtw_rust_hal_com_tx_aclt_conf_at(void *p, u8 idx)
{
	return idx >= TX_ACLT_CONF_NUM ? 0 : &adapter_to_dvobj(to_adap(p))->tx_aclt_confs[idx];
}

u8 rtw_rust_hal_com_tx_aclt_flags(void *p)
{
	return adapter_to_dvobj(to_adap(p))->tx_aclt_flags;
}

u8 rtw_rust_hal_com_tx_aclt_read8(void *p, u32 addr)
{
	(void)p;
	(void)addr;
	return 0;
}

u32 rtw_rust_hal_com_tx_aclt_read32(void *p, u32 addr)
{
	(void)p;
	(void)addr;
	return 0;
}

void rtw_rust_hal_com_tx_aclt_write8(void *p, u32 addr, u8 val)
{
	(void)p;
	(void)addr;
	(void)val;
}

void rtw_rust_hal_com_tx_aclt_write32(void *p, u32 addr, u32 val)
{
	(void)p;
	(void)addr;
	(void)val;
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