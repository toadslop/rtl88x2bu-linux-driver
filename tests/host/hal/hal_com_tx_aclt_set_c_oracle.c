// SPDX-License-Identifier: GPL-2.0
/* C oracle for W4-124 TX AC lifetime setters (from hal/hal_com_rest.c). */

#include "host_hal_com_tx_aclt_set_types.h"

#if CONFIG_TX_AC_LIFETIME

void rtw_hal_set_tx_aclt_force_val_c_oracle(struct _adapter *adapter,
					    struct tx_aclt_conf_t *input, u8 arg_num)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct tx_aclt_conf_t *conf = &dvobj->tx_aclt_force_val;

	if (arg_num >= 1) {
		if (input->en == 0xFF)
			conf->en = input->en;
		else
			conf->en = input->en & 0xF;
	}
	if (arg_num >= 2) {
		conf->vo_vi = input->vo_vi / (hal_spec->tx_aclt_unit_factor * 32);
		if (conf->vo_vi > 0xFFFF)
			conf->vo_vi = 0xFFFF;
	}
	if (arg_num >= 3) {
		conf->be_bk = input->be_bk / (hal_spec->tx_aclt_unit_factor * 32);
		if (conf->be_bk > 0xFFFF)
			conf->be_bk = 0xFFFF;
	}
}

void rtw_hal_set_tx_aclt_conf_c_oracle(struct _adapter *adapter, u8 conf_idx,
				       struct tx_aclt_conf_t *input, u8 arg_num)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct tx_aclt_conf_t *conf;

	if (conf_idx >= TX_ACLT_CONF_NUM)
		return;

	conf = &dvobj->tx_aclt_confs[conf_idx];

	if (arg_num >= 1) {
		if (input->en != 0xFF)
			conf->en = input->en & 0xF;
	}
	if (arg_num >= 2) {
		conf->vo_vi = input->vo_vi / (hal_spec->tx_aclt_unit_factor * 32);
		if (conf->vo_vi > 0xFFFF)
			conf->vo_vi = 0xFFFF;
	}
	if (arg_num >= 3) {
		conf->be_bk = input->be_bk / (hal_spec->tx_aclt_unit_factor * 32);
		if (conf->be_bk > 0xFFFF)
			conf->be_bk = 0xFFFF;
	}
}

void rtw_hal_update_tx_aclt_c_oracle(struct _adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u8 lt_en, lt_en_ori;
	u16 lt_vo_vi, lt_be_bk;
	u32 lt, lt_ori;

	lt_en_ori = rtw_rust_hal_com_tx_aclt_read8(adapter, REG_LIFETIME_EN);
	lt_ori = rtw_rust_hal_com_tx_aclt_read32(adapter, REG_PKT_LIFE_TIME);
	rtw_rust_hal_com_tx_aclt_compute_lt(adapter, &lt_en, &lt_vo_vi, &lt_be_bk);

	if (dvobj->tx_aclt_force_val.en != 0xFF)
		lt_en = dvobj->tx_aclt_force_val.en;
	if (dvobj->tx_aclt_force_val.vo_vi)
		lt_vo_vi = (u16)dvobj->tx_aclt_force_val.vo_vi;
	if (dvobj->tx_aclt_force_val.be_bk)
		lt_be_bk = (u16)dvobj->tx_aclt_force_val.be_bk;

	lt_en = (lt_en_ori & 0xF0) | (lt_en & 0x0F);
	lt = ((u32)lt_be_bk << 16) | lt_vo_vi;

	if (lt_en != lt_en_ori)
		rtw_rust_hal_com_tx_aclt_write8(adapter, REG_LIFETIME_EN, lt_en);
	if (lt != lt_ori)
		rtw_rust_hal_com_tx_aclt_write32(adapter, REG_PKT_LIFE_TIME, lt);
}

#endif /* CONFIG_TX_AC_LIFETIME */