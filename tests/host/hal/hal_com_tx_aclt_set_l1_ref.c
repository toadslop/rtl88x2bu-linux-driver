// SPDX-License-Identifier: GPL-2.0
/* L1 stubs for W4-124 TX AC lifetime setters (Rust body in hal_com_tx_aclt_set.rs). */

struct _adapter;
struct tx_aclt_conf_t;

void rtw_hal_set_tx_aclt_force_val(struct _adapter *adapter, struct tx_aclt_conf_t *input,
				   unsigned char arg_num)
{
	(void)adapter;
	(void)input;
	(void)arg_num;
}

void rtw_hal_set_tx_aclt_conf(struct _adapter *adapter, unsigned char conf_idx,
			      struct tx_aclt_conf_t *input, unsigned char arg_num)
{
	(void)adapter;
	(void)conf_idx;
	(void)input;
	(void)arg_num;
}

void rtw_hal_update_tx_aclt(struct _adapter *adapter)
{
	(void)adapter;
}