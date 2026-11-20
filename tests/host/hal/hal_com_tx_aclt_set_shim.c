// SPDX-License-Identifier: GPL-2.0
#include "host_hal_com_tx_aclt_set_types.h"
#include <string.h>

static struct _adapter g_adapter;

struct host_tx_aclt_reg_trace host_tx_aclt_reg_trace;

void host_tx_aclt_reset(void)
{
	memset(&g_adapter, 0, sizeof(g_adapter));
	memset(&host_tx_aclt_reg_trace, 0, sizeof(host_tx_aclt_reg_trace));
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

u8 rtw_rust_hal_com_tx_aclt_ap_m2u_num(void *p)
{
	return adapter_to_dvobj(to_adap(p))->host_ap_m2u_num;
}

u8 rtw_rust_hal_com_tx_aclt_sta_op_num(void *p)
{
	return adapter_to_dvobj(to_adap(p))->host_sta_op_num;
}

void rtw_rust_hal_com_tx_aclt_compute_lt(void *p, u8 *lt_en, u16 *lt_vo_vi, u16 *lt_be_bk)
{
	struct dvobj_priv *dv = adapter_to_dvobj(to_adap(p));
	int i;

	if (!lt_en || !lt_vo_vi || !lt_be_bk)
		return;

	*lt_en = 0;
	*lt_vo_vi = 0xFFFF;
	*lt_be_bk = 0xFFFF;
	for (i = 0; i < TX_ACLT_CONF_NUM; i++) {
		struct tx_aclt_conf_t *conf;

		if (!(dv->tx_aclt_flags & (1u << i)))
			continue;
		conf = &dv->tx_aclt_confs[i];
		if (i == TX_ACLT_CONF_DEFAULT) {
			*lt_en = conf->en;
			if (conf->vo_vi)
				*lt_vo_vi = (u16)conf->vo_vi;
			if (conf->be_bk)
				*lt_be_bk = (u16)conf->be_bk;
		} else if (i == TX_ACLT_CONF_AP_M2U && dv->host_ap_m2u_num &&
			   dv->host_sta_op_num) {
			u32 vo = *lt_vo_vi;
			u32 be = *lt_be_bk;

			*lt_en |= conf->en;
			if (conf->vo_vi && vo > conf->vo_vi)
				vo = conf->vo_vi;
			if (conf->be_bk && be > conf->be_bk)
				be = conf->be_bk;
			*lt_vo_vi = (u16)vo;
			*lt_be_bk = (u16)be;
		}
	}
}

u8 rtw_rust_hal_com_tx_aclt_read8(void *p, u32 addr)
{
	(void)p;
	if (addr == REG_LIFETIME_EN)
		return host_tx_aclt_reg_trace.reg_lt_en;
	return 0;
}

u32 rtw_rust_hal_com_tx_aclt_read32(void *p, u32 addr)
{
	(void)p;
	if (addr == REG_PKT_LIFE_TIME)
		return host_tx_aclt_reg_trace.reg_lt;
	return 0;
}

void rtw_rust_hal_com_tx_aclt_write8(void *p, u32 addr, u8 val)
{
	(void)p;
	if (addr != REG_LIFETIME_EN)
		return;
	host_tx_aclt_reg_trace.write8_count++;
	host_tx_aclt_reg_trace.last_lt_en = val;
}

void rtw_rust_hal_com_tx_aclt_write32(void *p, u32 addr, u32 val)
{
	(void)p;
	if (addr != REG_PKT_LIFE_TIME)
		return;
	host_tx_aclt_reg_trace.write32_count++;
	host_tx_aclt_reg_trace.last_lt = val;
}

#ifndef RUST_HAL_COM_TX_ACLT_SET_ORACLE
void rtw_hal_set_tx_aclt_force_val_c_oracle(struct _adapter *, struct tx_aclt_conf_t *, u8);
void rtw_hal_set_tx_aclt_conf_c_oracle(struct _adapter *, u8, struct tx_aclt_conf_t *, u8);
void rtw_hal_update_tx_aclt_c_oracle(struct _adapter *);

void rtw_hal_set_tx_aclt_force_val(struct _adapter *a, struct tx_aclt_conf_t *in, u8 n)
{
	rtw_hal_set_tx_aclt_force_val_c_oracle(a, in, n);
}

void rtw_hal_set_tx_aclt_conf(struct _adapter *a, u8 idx, struct tx_aclt_conf_t *in, u8 n)
{
	rtw_hal_set_tx_aclt_conf_c_oracle(a, idx, in, n);
}

void rtw_hal_update_tx_aclt(struct _adapter *a)
{
	rtw_hal_update_tx_aclt_c_oracle(a);
}
#endif