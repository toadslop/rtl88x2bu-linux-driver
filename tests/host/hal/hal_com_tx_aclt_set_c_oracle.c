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

#endif /* CONFIG_TX_AC_LIFETIME */